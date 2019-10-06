<?php
/*Skript pro restart databaze*/
session_start();
session_destroy();
include "mysql.php";
/* restart */

$zaloha = file_get_contents("sql/referencni_stav.sql");

if(mysqli_multi_query($db, $zaloha)){
    if (isset($_POST["page"]))
        header("refresh:3; url=index.php?msg=199&pg=" . $_POST["page"]);
    else
        header("refresh:3; url=index.php?msg=199");
} else {
    if (isset($_POST["page"]))
        header("refresh:3; url=index.php?msg=98&pg=" . $_POST["page"]);
    else
        header("refresh:3; url=index.php?msg=98");
}
echo "Restartuji db....";
/* presmerovani */
?>