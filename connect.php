<?php
class dtdb{
 public $link='';
 function __construct($temperature, $humidity, $altitude, $pressure, $sealevelpressure, $soilhumidity, $rain, $lightlux){
  $this->connect();
  $this->storeInDB($temperature, $humidity, $altitude, $pressure, $sealevelpressure, $soilhumidity, $rain, $lightlux);
 }
 
 function connect(){
  $this->link = mysqli_connect('localhost','root','') or die('Cannot connect to the DB');
  mysqli_select_db($this->link,'gms') or die('Cannot select the DB');
 }
 
 function storeInDB($temperature, $humidity, $altitude, $pressure, $sealevelpressure, $soilhumidity, $rain, $lightlux){
  $query =  "INSERT INTO gms_data ".
  "(temperature,humidity,altitude,pressure,sealevelpressure,soilhumidity,rain,lightlux) "."VALUES ".
  "('$temperature','$humidity','$altitude','$pressure','$sealevelpressure','$soilhumidity','$rain','$lightlux')";
  $result = mysqli_query($this->link,$query) or die('Errant query:  '.$query);
 }
 
}
if($_GET['temperature'] != '' and  $_GET['humidity'] != '' and  $_GET['altitude'] != '' and  $_GET['pressure'] != '' and  $_GET['sealevelpressure'] != '' and  $_GET['soilhumidity'] != '' and  $_GET['rain'] != '' and  $_GET['lightlux'] != ''){
 $dtdb=new dtdb($_GET['temperature'], $_GET['humidity'], $_GET['altitude'], $_GET['pressure'], $_GET['sealevelpressure'], $_GET['soilhumidity'], $_GET['rain'], $_GET['lightlux']);
}
?>