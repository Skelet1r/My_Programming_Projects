<?php

const SERVER_NAME = "db";
const USERNAME = "root";
const PASSWORD = "root";
const DB_NAME = "forum";

$conn = new mysqli(SERVER_NAME, USERNAME, PASSWORD, DB_NAME);

if ($conn->connect_error) {
    die($conn->connect_error);
}

return $conn;