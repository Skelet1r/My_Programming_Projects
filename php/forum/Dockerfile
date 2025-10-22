FROM php:8.4-apache

RUN docker-php-ext-install mysqli pdo pdo_mysql
RUN a2enmod rewrite

RUN curl -sS https://getcomposer.org/installer | php -- \
--install-dir=/usr/local/bin --filename=composer

COPY public/       /var/www/html/
COPY config/       /var/www/config/
COPY src/          /var/www/src/
COPY composer.json /var/www/
COPY Views/        /var/www/Views/

RUN composer dump-autoload -d /var/www

WORKDIR /var/www/html