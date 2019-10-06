<?php
/*Skript pro odhlaseni uzivatele*/
session_start();
$_SESSION["logged"] = 0;
session_destroy();
if (isset($_POST["page"]))
    header("location: index.php?msg=103&pg=" . $_POST["page"]);
else
    header("location: index.php?msg=103");
?>