<?
/*Skript pro smazani zamestnance*/
session_start();
include "mysql.php";
if ($_SESSION['role'] > 1) {
    $z_id = $_GET["id"];
    $z_prava = $_GET["prava"];
    if ($z_prava == 0) { //administratora nelze smazat
        if (mysqli_query($db, "DELETE FROM zamestnanec WHERE id='$z_id'")) {
            header("location: index.php?pg=rm&co=zamestnanec&msg=122");
        } else {
            header("location: index.php?pg=rm&co=zamestnanec&msg=99");
        }
    } else
        header("location: index.php?pg=rm&co=zamestnanec&msg=45");
} else {
    header("location: index.php");
}

?>