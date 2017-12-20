<?php
header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Headers: access");
header("Access-Control-Allow-Methods: GET");
header("Access-Control-Allow-Credentials: true");
header("Content-Type: application/json; charset=UTF-8");
 
// include database and object files
include_once '../config/database.php';
include_once '../objects/deseo.php';
 
// get database connection
$database = new Database();
$db = $database->getConnection();
 
// prepare deseo object
$deseo = new Deseo($db);
 
// set ID property of deseo to be edited
$deseo->id = isset($_GET['id']) ? $_GET['id'] : die();
 
// read the details of deseo to be edited
$deseo->readOne();
 
// create array
$deseo_arr = array(
    "id" =>  $deseo->id,
    "color" => $deseo->color,
    "ttl" => $deseo->ttl,
    "date_add" => $deseo->date_add 
);
 
// make it json format
print_r(json_encode($deseo_arr));
?>