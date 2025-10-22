<?php 
//скрипт оплаты по токенам киви
ini_set('memory_limit', '7095M');
$from = strtotime('2021-11-12 13:30:00');
$to   = strtotime('2021-11-12 14:00:00'); 
$now = time();
if ($now > $from && $now < $to) {
	print 'wait to ' . $to;
	die();
}

chdir('domains/everwell.ru/public_html/include/cron/payture');
include('settings.php');
require("/payture.php");

set_time_limit(5000);

class recurrentCron {
	public $db;
	public $frod_response_codes;//коды ответов, относящиеся к фроду.
	public $stream_num; //id потока (для работы нескольких)
	public $SCRIPT_END_TIME;
	
	function __construct ($stream_num) {
		$this->db = utb24::utb24_db();
		$this->stream_num = $stream_num;
		/*
		** время работы скрипты не считается при ожидании ответа от другого сервера,
		** по этой причине мы жетско фиксируем время работы скрипты 
		*/
		$this->SCRIPT_END_TIME = time() + 5000;//максимальное время работы скрипта 120 секунд
	}
	
	/*
	** Жестко фиксируем время работы скрипта
	** если скрипт работает дольше, чем положенно - завершаем
	*/
	public function fix_time () {
		if (time() >= $this->SCRIPT_END_TIME) {
			print "END TIME";
			_alert(__FILE__, "Превышение времени работы скрипта");
			die();
		}	
	}
	
	/*
	** Функция возвращает заполненный объект подписчика
	*/
	public function get_sub_from_db () {
		$time = time();
		$select_query = "SELECT * FROM utb24_fondy_registration WHERE next_transaction_date <= ${time} && ready_for_pay=1 && stream_id={$this->stream_num} ORDER BY created DESC LIMIT 0, 1;";
		$res = $this->db->query($select_query); 
		
		if (!$res) {
			echo "Error: " . $query . "<br>" . $this->db->error;
			die();
		}
		$row = $res->fetch_assoc();
		if (!$row) {
		  return false;	
		}
		
		$subObj = new subscriber();
		$subObj->add_raw_row($row);
		//$row['order_data'] = unserialize($row['order_data']);
		//$row['unitpay_response'] = unserialize($row['unitpay_response']);
		
		return $subObj;
	}
	
	
	public function start_transactions () {
		$transactions = new transactions();
		for ($i = 0; $i<20; $i++){
			bdbg($i);
			/* Жестко фиксируем время работы скрипта*/
			$this->fix_time();
			
			$subObj = $this->get_sub_from_db();
		
			/*очередь пуста*/
			
			if ($subObj == false) {
			  bdbg("Stack is empty");
			  return 0;	
			}
			
			
			$sub = $subObj->sub;//объект пользователя
			
			$aid = $sub['aid'];
			
//			//если нет списаний в течении 200 дней, отписка
//			if ( $subObj->no_success_in_period(500) ) {
//				$subObj->unsubscribe(true);
//				continue;
//			}
			
			/*сразу ставим следующее списание через 12 часов*/
			$next_transaction_date_safe = time() + 60 * 60 * 12; 
			$this->db->query("
				UPDATE utb24_fondy_registration SET next_transaction_date = {$next_transaction_date_safe}
				WHERE aid = {$aid}
			");
			
			global $payture;
			//сумма по умолчанию
			$amount = $payture->Subtotal;
			$type = 2;
			//получаем сумму платежа
			$last = $subObj->get_last_transaction();
			
			//выставляем статус ожидание, в киви не работает
			if ($last['status'] == 'pending') {
				
			}
			
			//если последнаяя транзакция - привязка карты
			if ($last['t'] == 1) {
				$amount = $payture->Subtotal;
				$type = 2;
			}
			
			//если последнаяя транзакция - первая сумма (399) и успешная
			//Это значит, что следующая сумма списания будет такой же сумма типа 2 и через 5 дней
			if ($last['t'] == 2 && $last['status'] == 'success') {
				$amount = $payture->Subtotal;
				$type = 2;
			}
			//если последнаяя транзакция - первая сумма (399) и неуспешная
			//Это значит, что следующая сумма списания будет сумма типа 3 (49 руб)
			if ($last['t'] == 2 && $last['status'] != 'success') {
				$amount = $payture->Subtotal_min;
				$type = 3;
			}
			
			//если последнаяя транзакция - сумма типа №3 (49 руб)
			//то следующая будет опять сумма типа 2 (399) через 5 дней
			if ($last['t'] == 3) {
				$amount = $payture->Subtotal;
				$type = 2;
			}
			
			//если последнаяя транзакция - произошел какой-то фрод,
			//то следующая транзакция будет опять сумма типа 2 (399)
			if ($last['t'] == 4) {
				$amount = $payture->Subtotal;
				$type = 2;
			}
			
			//если после последнего списания прошло более 16 часов, списываем 399
			if (time() - $last['timestamp'] > 60 * 60 * 16) {
				$amount = $payture->Subtotal;
				$type = 2;
			}
			
			//получаем сумму успешных списаний за 31 день
			$last_period_sum = $subObj->get_last_peroid_transactions_sum (60 * 60 *24 * 31, 'success');
			//сумма всех успешных списаний
			$total_sum = $subObj->get_last_peroid_transactions_sum (60 * 60 *24 * 9999, 'success');
			
			
			
//			/*
//			** на всякий случай,если по какой-то причине не сработают фильтры
//			*/
//			if ($total_sum > 3399) {
//				$subObj->unsubscribe(true);
//				_alert(__FILE__, "У пользователя aid={$aid} total_sum = {$total_sum}!");
//				continue;
//			}
//
//			if ($last_period_sum > 1000) {
//				//$subObj->unsubscribe(true);
//				_alert(__FILE__, "У пользователя aid={$aid} last_period_sum = {$last_period_sum}!");
//				continue;
//			}
			
//			/*
//			** если итоговая сумма списаний 1948 и выше,
//			** то отписываем.
//			*/
//			if ($total_sum > 3399) {
//				$subObj->unsubscribe(true);
//				continue;
//			}
			
			
//			/*
//			Проверяем общую сумму упешных списаний
//			Если за последние 31 день сумма составила >951, то ставим следующее списание через сутки
//			и так они будут проверятся до тех пор, пока не пройдет 31 день
//			*/
//			if ($last_period_sum > 1000 - $payture->Subtotal_min) {
//				$subObj->sub['next_transaction_date'] = time() + 60 * 60 * 24;
//				$subObj->update_sub();
//				continue;
//			}
//
//			//если нужно списать сумму менее 49, то переносим транзакцию на следующий день
//			//и так продолжается перенос, пока не пройдет определенный период
//			if ($last_period_sum < 1000 && 1000 - $last_period_sum < $payture->Subtotal_min) {
//				//$subObj->sub['next_transaction_date'] = time() + 60 * 60 * 24;
//				//$subObj->update_sub();
//				//continue;
//			}
			
			
//			/*
//			проверка на превышение тотала 3399
//			*/
//			if ($amount + $total_sum >= 3399) {
//				$amount = 3399 - $total_sum;
//				//если лимит близок, отписываем
//				if ($amount < $payture->Subtotal_min) {
//					$subObj->unsubscribe(true);
//					continue;
//				}
//			}
//
//
//			/*
//			если сумма списания может привести к превышению месячного лимита,
//			то уменьшаем сумму
//			*/
//			if ($amount + $last_period_sum > 1000) {
//				$amount = 1000 - $last_period_sum;
//			}
//
//			/*
//			если сумма списания меньше 49 рублей до месячного лимита,
//			то откладываем на следующий день
//			*/
//			if ($amount < $payture->Subtotal_min) {
//				$subObj->sub['next_transaction_date'] = time() + 60 * 60 * 24;
//				$subObj->update_sub();
//				continue;
//			}
			
			//если был превышен лимит обращений за сутки
			if ($subObj->today_transactions_limit() ) {
				//$subObj->sub['next_transaction_date'] = time() + 60 * 60 * 24 * 3;
				//$subObj->update_sub();
				//continue;
			}
			
			//ставим следующую транзакцию через 5 дней
			//$subObj->sub['next_transaction_date'] = time() + $payture->transaction_interval;
			$order_id = $subObj->generate_order_id();


			//$subObj->add_transaction(time(), 8383, 45, 'error', 2);
			
			//добавляем транзакцию в статус ожидание
			//$subObj->add_transaction(time(), $order_id, $amount, 'pending', $type);
			//$subObj->update_sub();
			
			//отправляем запрос на оплату токеном
			//$subObj->sub['order_data']['transactions'] = [];
			//$subObj->add_transaction(time(), $order_id, 1, 'success', 1);
			//$subObj->update_sub();
			if (!isset($subObj->sub['order_data']['paymentToken'])) {
				$subObj->unsubscribe(true);
				continue;
			} 
			$pay_token = $subObj->sub['order_data']['paymentToken'];
			$result = payture_recurent ($aid, $aid, $pay_token, $order_id, $amount, $subObj->get_cheque_contacts(), $type);

			
			$recurent_status = $result['@attributes']['Success'];
			bdbg($amount);
			bdbg($result);
			bdbg("--------------------------------");
			bdbg(date("Y-m-d H:i:s", $subObj->sub['next_transaction_date']));
			bdbg($subObj->sub);
			bdbg("*****************************");
			bdbg("*****************************");

			//если оплата прошла
			if ($recurent_status == "True") {
				$subObj->add_transaction(time(), $order_id, $amount, 'success', $type);//добавляем транзакцию в список транзакций
				$subObj->sub['next_transaction_date'] = time() + $payture->transaction_interval;//дата следующего платежа
				$subObj->optimize_transactions_blob();
				$subObj->update_sub();
				$transactions->log_transaction ($aid, $order_id, $amount, time(), 'success', 'payture');//Логируем транзакцию
				continue;
			}
			
			$recurent_errcode = $result['@attributes']['ErrCode'];
			$this->db->query("INSERT INTO `err_transactions_log`(`aid`, `error`) VALUES ({$aid},'{$recurent_errcode}')");
			
			
			//если произошла ошибка, при которой делается отписка - отписываем
			if ($recurent_status == "False" && in_array($recurent_errcode, $payture->unsubscribe_codes)) {
				$subObj->add_transaction(time(), $order_id, $amount, 'frod', $type);//добавляем транзакцию в список транзакций
				$subObj->sub['ready_for_pay'] = 0;
				$subObj->sub['unsubscribe_date'] = time();
				$subObj->sub['unsubscribe_reason'] = 'frod';
				$subObj->optimize_transactions_blob();
				$subObj->update_sub();
				$transactions->log_transaction ($aid, $order_id, $amount, time(), 'frod', 'payture');//Логируем транзакцию
				continue;
			}
			
//			//если произошел фрод
//			if ($recurent_status == "False" && in_array($recurent_errcode, $payture->fraud_codes)) {
//				$subObj->add_transaction(time(), $order_id, $amount, 'frod', $type);//добавляем транзакцию в список транзакций
//				$subObj->sub['next_transaction_date'] = time() + $payture->fraud_interval;//дата следующего платежа
//				$subObj->optimize_transactions_blob();
//				$subObj->update_sub();
//				$transactions->log_transaction ($aid, $order_id, $amount, time(), 'frod', 'payture');//Логируем транзакцию
//				if ($subObj->is_last_3_frauds() == true) {
//					$subObj->unsubscribe(true, 'frod');
//				}
//				continue;
//			}
			
			//если произошла другая ошибка (нет денег и т.д.)
			if ($recurent_status == "False") {
				$subObj->add_transaction(time(), $order_id, $amount, 'error', $type);//добавляем транзакцию в список транзакций
				$subObj->sub['next_transaction_date'] = payture_next_transaction_date($subObj->sub['order_data']['transactions'], $subObj->sub['card_mask']);//следующая дата списания в зависимости от условий (тип карты, кол-во запросов)
				$subObj->optimize_transactions_blob();
				$subObj->update_sub();
				$transactions->log_transaction ($aid, $order_id, $amount, time(), 'error', 'payture');//Логируем транзакцию
				
				
				continue;
			}
			
		}
	}
	
	


	public function calculateSum ($transactions, $write_off_sum) {
		$max_sum = 3399;
		$transactions_sum = 0;
		foreach ($transactions as $t) {
			if ($t['status'] != 'success') {
				continue;
			}
			$transactions_sum += $t['amount'];
			
		}
		$residue = $max_sum - $transactions_sum;
		if ($residue >= $write_off_sum) {
			return $write_off_sum;
		}
		if ($residue < $write_off_sum && $residue > 0) {
			return $residue;
		}
		if ($residue <= 0) {
			return false;
		}
		
	}

}



$stream_num = (int) $argv[1];
//payture 200 - 207
if ($stream_num < 99 || $stream_num > 500) {
	print "WRONG STREAM NUMBER";
	exit;	
}

//устанавливаем ограничение на запуск скрипта
$runtime_limit = new runtime_limit("../../tmp/cron_${stream_num}_timelimit.txt", 5000);
$runtime_limit->run();
$cron = new recurrentCron($stream_num);
$cron->start_transactions();  
//$cron->db->close();
//снимаем ограничение
$runtime_limit->finish();
?>