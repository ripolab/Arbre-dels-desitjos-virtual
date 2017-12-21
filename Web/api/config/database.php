<?php
//https://www.codeofaninja.com/2017/02/create-simple-rest-api-in-php.html
class Database{
 
    // specify your own database credentials
    private $host = "";     // ip o url del servidor
    private $db_name = "";  // nombre de la bbdd
    private $username = ""; // usuario de la bbdd
    private $password = ""; // password de la bbdd
    public $conn;
 
    // get the database connection
    public function getConnection(){
 
        $this->conn = null;
 
        try{
            $this->conn = new PDO("mysql:host=" . $this->host . ";dbname=" . $this->db_name, $this->username, $this->password);
            $this->conn->exec("set names utf8");
        }catch(PDOException $exception){
            echo "Connection error: " . $exception->getMessage();
        }
 
        return $this->conn;
    }
}
?>