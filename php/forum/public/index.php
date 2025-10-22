<?php
require __DIR__ . "/../vendor/autoload.php";

use App\Router;

$request = $_SERVER['REQUEST_URI'];

switch ($request) {
    case "/":
        require __DIR__ . '/../Views/home.php';
        break;
    case "/registration":
        require __DIR__ . '/../Views/registration.php';
        break;
    // case "/registration":
    //     require __DIR__ . '/../src/User/registration.php';
    //     break;
    case "/login":
        require __DIR__ . '/../Views/login.php';
        break;
}