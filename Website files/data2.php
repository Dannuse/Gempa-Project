<?php
//Define database host and login details
$host='localhost';
$user='id4670621_gempa2018';
$pass='gempa2018';
$db='id4670621_gempadb';

//Connect to the database
$con=mysqli_connect($host,$user,$pass,$db);
if($con)
echo 'Connected successfully to database ';
else
echo 'Cannot connect to database ';

//Add data from Arduino to the database table
$sql = "INSERT INTO gempatable (date,time,coordinates) VALUES ('".$_GET["date"]."','".$_GET["time"]."','".$_GET["coordinates"]."')";

$query=mysqli_query($con,$sql);
if($query)
echo 'Data inserted successfully ';
else
echo 'Data not inserted ';

?>

