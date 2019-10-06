<?php
/*Skript pro pridani vytisku do knihovny*/
session_start();
include "mysql.php";
//pomocne promenne
if ($_SESSION['role'] > 1) {
    $error = 0;
    //zpracovani udaju z formulare
    $pocet = mysqli_real_escape_string($db, $_POST["pocet"]);
    if (!is_numeric($pocet) || intval($pocet) > 20 || intval($pocet) < 1)
        $error = 26; //spatne zapsany pocet
    $t_id = mysqli_real_escape_string($db, $_POST["titul"]);
    echo $t_id . "<br>" . $pocet . "<br>" . $error;
    //vlozeni
    if ($error == 0) {
        //vlozeni vytisku do db
        for ($a = 0; $a < $pocet; $a++)
            if(!mysqli_query($db, "INSERT INTO vytisk (id_titul, dostupnost) VALUES ('$t_id','1')")){
                header("location: index.php?msg=99");
            }
        //presmerovani
        header("location: index.php?msg=111");
    } else //navrat na vlozeni vytisku s chybou
    {
        header("location: index.php?pg=add&co=vytisk&msg=" . $error);
    }
} else { //ty tu nemas co delat
    header("location: index.php");
}
?>