<?php
class Deseo {
 
    // database connection and table name
    private $conn;
    private $table_name = "deseos";
 
    // object properties
    public $id;
    public $color;
    public $ttl;
    public $date_add;
 
    // constructor with $db as database connection
    public function __construct($db){
        $this->conn = $db;
    }
    
    // read deseos
    function read(){
     
        // select all query
        $query = "SELECT * FROM " . $this->table_name . " ORDER BY date_add ASC";
     
        // prepare query statement
        $stmt = $this->conn->prepare($query);
     
        // execute query
        $stmt->execute();
     
        return $stmt;
    }
    
    // used when filling up the update product form
    function readOne(){
     
        // query to read single record
        $query = "SELECT
                    *
                FROM
                    " . $this->table_name . " 
                WHERE
                    id = ?
                LIMIT
                    0,1";
     
        // prepare query statement
        $stmt = $this->conn->prepare( $query );
     
        // bind id of deseo to be updated
        $stmt->bindParam(1, $this->id);
     
        // execute query
        $stmt->execute();
     
        // get retrieved row
        $row = $stmt->fetch(PDO::FETCH_ASSOC);
     
        // set values to object properties
        $this->id = $row['id'];
        $this->color = $row['color'];
        $this->ttl = $row['ttl'];
        $this->date_add = $row['date_add'];
    }
    
    // used when filling up the update product form
    function readLast() {
        
        $query = "SELECT max(id) FROM " . $this->table_name;
        
        // prepare query statement
        $stmt = $this->conn->prepare($query);
     
        // execute query
        $stmt->execute();
        
        $row = $stmt->fetch(PDO::FETCH_ASSOC);
        
        return $row["max(id)"];
    }
}