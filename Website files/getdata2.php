<?php
$url=$_SERVER['REQUEST_URI'];
//Refresh the webpage every 5 seconds to uptdate database
header("Refresh: 5; URL=$url");  
?>

<!-- HTML code to display information neatly -->
<html>
<head>
    <style>
table, th, td {
    border: 1px solid black;
    padding: 50px;
    text-align: center;
    background-color: #c0c0c0;
    color: black;
    height: 50px;
    vertical-align: bottom;
    font-size:25px;
    border-radius: 25px;
    box-shadow: 10px 10px 8px 10px #000000;
    }


body {
    background-image: url("assets/images/paper.jpg");
}
#gps p { 
    color:red;
    font-size:20px;
    box-shadow: 5px 10px #888888;
    
    
}  
div.header{
    height:100px;
    width:100%;
    background: radial-gradient(white 5%, lightblue 15%, blue 80%);
    position: relative;
    top:-20px;
    left: -5px;
    width: 100%;
    border: 3px solid black;
}

div.footer {
    position: relative;
    top: 700px;
    left: -5px;
    background: radial-gradient(white 5%, lightblue 15%, blue 80%);
    width: 100%;
    height: 100px;
    border: 3px solid black;
}


#greeting p {
    background-color:#236589;  
    color:white;  
    padding: 10px 30px 25px 30px; 
    text-align:center; 
    width:500px;
    height:10px;
    font-size:30px;
    border-radius: 25px;
    box-shadow: 5px 10px #888888;
    
}
#sidediv {
    position: absolute;
    top: 250px;
    left:50px;
    width: 200px;
    height: 500px;
    border: 3px solid black;
}  
 #mess p{
            color:black;
            font-size:25px;
            background-color:#c0c0c0; 
            border: 2px solid #0066FF; 
            padding:25px;
            width:500px;
            border-radius: 25px;
            box-shadow: 5px 10px #000000;
        }
     </style>
    <title>GPS Data</title>
    </head>
    <body>
         <div class="header">
       
    </div>
    
    
       <div align="center" id="message"> 
       <marquee>GSM Enabled Mobile Panic Alarm</marquee>
        <div id="greeting">
            
        <p>Welcome to our GPS Database</p>
        </div>
        <div id="mess">
            <div class="message"><br>
            
                
                <p class="gps">Click on the GPS corrdinates to go to Google maps. </p>
              </div>
        </div>
       
        <table>
            <thead>
            <tr>
	            <!-- Database Headings -->
    	        <th bgcolor="#5D7B9D"><font face="Courier New" color="blue" size="6px">Date</font></th>
    	        <th bgcolor="#5D7B9D"><font face="Courier New" color="blue" size="6px">Time</font></th>
                <th bgcolor="#5D7B9D"><font face="Courier New" color="blue" size="6px">Coordinates & Status</font></th>
                

            </tr>
            </thead>
        </div> 
<?php
    // Connect to database

   // IMPORTANT: If you are using XAMPP you will have to enter your computer IP address here, if you are using webpage enter webpage address (ie. "www.yourwebpage.com")
    $con=mysqli_connect("localhost","id4670621_gempa2018","gempa2018","id4670621_gempadb");
       
    // Retrieve data from database table and display them   
    $result = mysqli_query($con,"SELECT * FROM `gempatable`");
      
    // Process every record
    while($row = mysqli_fetch_array($result))
    //If data matches these sentences, dont highlight them or make them clickable
    {
    echo "<tr>";
 	echo "<td>" . $row['date'] . "</td>";
 	echo "<td>" . $row['time'] . "</td>";
 	
 	if($row['coordinates'] == "Cannot find GPS coordinates")
 	{
    echo "<td>" . $row['coordinates'] . "</td>";
 	}
    else if($row['coordinates'] == "Panic alarm was Activated")
 	{
    echo "<td>" . $row['coordinates'] . "</td>";
 	}
 	else if($row['coordinates'] == "False alarm message was Activated")
 	{
    echo "<td>" . $row['coordinates'] . "</td>";
 	}
 	else if($row['coordinates'] == "Reset button was Activated")
 	{
    echo "<td>" . $row['coordinates'] . "</td>";
 	}
 	//Make the following sentence clickable and open the website link
  	else if($row['coordinates'] == "Credit balance is getting low")
 	{
    echo "<td>"."<a href = https://www.48months.ie/auth/login>" .$row['coordinates']."</td>";
        echo "</tr>";
 	}
 	//Open the GPS coordinates in google maps
 	else{
 	    echo "<td>"."<a href = https://www.google.ie/maps/place/".$row['coordinates'] . "/a>" .$row['coordinates']."</td>";
        echo "</tr>";
 	}
    }
    
    // Close the connection   
    mysqli_close($con);
?>
    <!-- Click on link to return to the website -->
    </table>
    <div class="footer">
       
    </div>
   <a class="nav-link" href="https://gempapanicalarm.000webhostapp.com">Return to GEMPA Website</a>
    </body>
</html>
