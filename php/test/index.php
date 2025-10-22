<?php
$request = $_SERVER["REQUEST_URI"];

switch ($request) {
    case "/":
        require "./home.php";
        break;
    case "/login":
        require "./login.php";
        break;
    case "/registration":
        require "./registration.php";
        break;
}