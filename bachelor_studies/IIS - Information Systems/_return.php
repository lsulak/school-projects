<?php
/* Skript pro vraceni vytisku */
session_start();
include 'mysql.php';
if ($_SESSION['role'] > 1) {
    $p_id = $_GET['id'];
    $pred = mysqli_query($db, "SELECT id_vytisk FROM vypujcka WHERE id='$p_id'");
    $predc = mysqli_fetch_assoc($pred);
    $v_id = $predc['id_vytisk'];
    if(mysqli_query($db, "DELETE FROM vypujcka WHERE id='$p_id'")){
        if(mysqli_query($db, "UPDATE vytisk SET dostupnost='1' WHERE id='$v_id'")){
            header("location: index.php?pg=man&co=vypujcka&msg=120");
        } else {
            header("location: index.php?pg=man&co=vypujcka&msg=99");
        }
    } else {
        header("location: index.php?pg=man&co=vypujcka&msg=99");
    }
} else {
    header("location: index.php");
}
?>