<?php
// required headers
header("Access-Control-Allow-Origin: *");
header("Content-Type: application/json; charset=UTF-8");
 
// include database and object files
include_once '../config/database.php';
include_once '../objects/deseo.php';
 
// instantiate database and deseo object
$database = new Database();
$db = $database->getConnection();
 
// initialize object
$deseo = new Deseo($db);
 
// query deseos
$stmt = $deseo->read();
$num = $stmt->rowCount();
 
// check if more than 0 record found
if($num>0){
 
    // deseos array
    $deseos_arr=array();
    $deseos_arr["records"]=array();
 
    // retrieve our table contents
    // fetch() is faster than fetchAll()
    // http://stackoverflow.com/questions/2770630/pdofetchall-vs-pdofetch-in-a-loop
    while ($row = $stmt->fetch(PDO::FETCH_ASSOC)){
        // extract row
        // this will make $row['color'] to
        // just $color only
        extract($row);
 
        $deseo_item=array(
            "id" => $id,
            "color" => $color,
            "ttl" => $ttl,
            "date_add" => $date_add
        );
 
        array_push($deseos_arr["records"], $deseo_item);
    }
 
    echo json_encode($deseos_arr);
}
 
else{
    echo json_encode(
        array("message" => "No hay deseos.")
    );
}
?>