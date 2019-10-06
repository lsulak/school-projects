<!DOCTYPE html>
<html>
<head>
    <title>Autori - knihovna ITI</title>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8">
    <meta name="description" content="Projekt do predmetu IIS">
    <meta name="author" content="Ladislav Šulák, Petr Švaňa">
    <link href="style.css" rel="stylesheet" type="text/css">
    <link rel="icon" href="images/book.ico">
    <script type="text/javascript" src="javascript/ucet.js"></script>
</head>
<body>
<a href="javascript:close()">[Zavřít okno]</a>
<br />
<br />

<?php
session_start();
include "../mysql.php";
$autori = mysqli_query($db, "SELECT prijmeni, jmeno FROM autor ORDER BY prijmeni");
while($row = mysqli_fetch_array($autori)){
    echo $row['jmeno']." ".$row["prijmeni"]."<br />";
}

?>
<br />
<a href="javascript:close()">[Zavřít okno]</a>
</body>
</html>