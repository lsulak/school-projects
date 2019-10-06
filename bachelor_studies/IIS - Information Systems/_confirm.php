<?php
/*Skript pro potvrzeni rezervace*/
session_start();
include 'mysql.php';

if ($_SESSION['role'] > 1) {
    if (!isset($_GET['id']))
        header("Location: index.php?pg=man&co=rezervace&msg=38");
    else {
        $r_id = $_GET['id'];
        $control = mysqli_query($db, "SELECT id FROM rezervace WHERE id='$r_id'");
        if (mysqli_num_rows($control) == 0)
            header("Location: index.php?pg=man&co=rezervace&msg=38");
        else {
            if(mysqli_query($db, "UPDATE rezervace SET stav='1' WHERE id='$r_id'")){
                header("Location: index.php?pg=man&co=rezervace&msg=113");
            } else {
                header("Location: index.php?pg=man&co=rezervace&msg=99");
            }

        }
    }
}
?>