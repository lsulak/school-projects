<?
/*Skript pro smazani vytisku*/
session_start();
include 'mysql.php';
if ($_SESSION['role'] > 1) {
    $v_id = $_GET["id"];

    $result = mysqli_query($db, "SELECT * FROM vypujcka WHERE id_vytisk='$v_id'");
    if (mysqli_num_rows($result) != 0) { //nelze smazat aktualne vypujceny vytisk
        header("location: index.php?pg=rm&co=vytisk&msg=47");
    } else {
        if(mysqli_query($db, "DELETE FROM vytisk WHERE id='$v_id'")){
            header("location: index.php?pg=rm&co=vytisk&msg=123");
        } else {
            header("location: index.php?pg=rm&co=vytisk&msg=99");
        }

    }
} else {
    header("location: index.php");
}
?>