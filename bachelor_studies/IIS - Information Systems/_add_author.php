<?php
/*Skript pro vlozeni autora do databaze*/
session_start();
include "mysql.php";
//role vetsi nez uzivatel
if ($_SESSION['role'] > 1) {
    $error = 0;
    //zpracovani udaju z formulare
    $prijmeni = (ucwords(strtolower(mysqli_real_escape_string($db, $_POST["prijmeni"]))));
    if (strlen($prijmeni) == 0)
        $error = 12; //nezadano prijmeni
    $jmeno = (ucwords(strtolower(mysqli_real_escape_string($db, $_POST["jmeno"]))));
    if (strlen($jmeno) == 0)
        $error = 11; //nezadano jmeno
    $nar = mysqli_real_escape_string($db, $_POST["nar"]);
    if (strlen($nar) == 0)
        $error = 23; //nezadano datum nar.
    if (strlen($nar) != 8 || !is_numeric($nar))
        $error = 24; //spatny format data
    $umr = mysqli_real_escape_string($db, $_POST["umr"]);
    if (strlen($umr) != 0) {//zadano datum
        if (strlen($umr) != 8 || !is_numeric($umr))
            $error = 25; //spatny format data
    } else {
        $umr = 0; //zrejme jeste neumrel
    }
    $edit = $_POST['edit'];
    //vlozeni
    if ($error == 0) {
        if ($edit == 0) {
            //vlozeni autora do db
            if (mysqli_query($db, "INSERT INTO autor (prijmeni, jmeno, datum_narozeni, datum_umrti) VALUES ('$prijmeni','$jmeno','$nar','$umr')")) {
                header("location: index.php?pg=tituly&msg=107");
            } else {
                header("location: index.php?pg=add&co=autor&msg=99&jmeno=" . $_POST["jmeno"] .
                    "&prijmeni=" . $_POST["prijmeni"] .
                    "&nar=" . $_POST["nar"] .
                    "&umr=" . $_POST["umr"]);
            }

        } else {// jedna se
            $a_id = $_POST['id'];
            //vlozeni autora do db
            if (mysqli_query($db, "UPDATE autor SET prijmeni='$prijmeni', jmeno='$jmeno', datum_narozeni='$nar', datum_umrti='$umr' WHERE id='$a_id'")) {
                header("location: index.php?msg=116");
            } else {
                header("location: index.php?pg=edit&co=autor&msg=99&id=" . $_POST['id']);
            }
        }
    } else //navrat na vytvoreni/editaci autora s chybou
    {
        if ($edit == 0) {
            header("location: index.php?pg=add&co=autor&msg=" . $error .
                "&jmeno=" . $_POST["jmeno"] .
                "&prijmeni=" . $_POST["prijmeni"] .
                "&nar=" . $_POST["nar"] .
                "&umr=" . $_POST["umr"]);
        } else {
            header("location: index.php?pg=edit&co=autor&msg=" . $error . "&id=" . $_POST['id']);
        }
    }
} else { //ty tu nemas co delat
    header("location: index.php");
}
?>