<?
/*Skript pro smazani autora z databaze*/
session_start();
include "mysql.php";
if ($_SESSION['role'] > 1) {
    $a_id = $_GET["id"];
    echo $a_id;
    if (mysqli_query($db, "DELETE FROM autor WHERE id='$a_id'")) {
        header("location: index.php?pg=rm&co=autor&msg=109");
    } else { //autor ma pridelene tituly
        header("location: index.php?pg=rm&co=autor&msg=34");
    }
} else {
    header("location: index.php");
}
?>