<?
/*Skript pro smazani titulu*/
session_start();
include "mysql.php";
if ($_SESSION['role'] > 1) {
    $t_id = $_GET["id"];
    if (mysqli_num_rows(mysqli_query($db, "SELECT * FROM vytisk WHERE id_titul='$t_id'")) != 0) {
        header("location: index.php?pg=rm&co=titul&msg=46"); //kontrola zda existuji vytisky
    } else {
        if (mysqli_query($db, "DELETE FROM autor_titul WHERE id_titul='$t_id'")) {
            if (mysqli_query($db, "DELETE FROM titul WHERE id='$t_id'")) {
                header("location: index.php?pg=rm&co=titul&msg=110");
            } else {
                header("location: index.php?pg=rm&co=titul&msg=99");
            }
        } else {
            header("location: index.php?pg=rm&co=titul&msg=99");
        }
    }
} else {
    header("location: index.php");
}
?>