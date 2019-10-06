<?php
/* Skript pro zmenu kontaktnich informaci */
session_start();
include 'mysql.php';
//pom. promenna
$error = 0;
//zpracovani formulare
$email = mysqli_real_escape_string($db, $_POST["email"]);
$adresa = mysqli_real_escape_string($db, $_POST["adresa"]);
$telefon = mysqli_real_escape_string($db, $_POST["telefon"]);
$heslo = mysqli_real_escape_string($db, $_POST["heslo"]);
$u_id = $_SESSION["logged"];
//kontrola na delku emailu
if (strlen($email) == 0)
    $error = 1; //nezadan email
//kontrola na pouziti emailu v tabulkach
if ($error == 0) {
    $email_zam = mysqli_query($db, "SELECT email FROM zamestnanec WHERE id NOT IN ( '$u_id' )");
    while ($row = mysqli_fetch_array($email_zam)) {
        if (strcasecmp($email, $row["email"]) == 0) {
            $error = 2;
            break;
        }
    }
    if ($error == 0) {
        $email_uzi = mysqli_query($db, "SELECT email FROM ctenar WHERE id NOT IN ( '$u_id' )");
        while ($row = mysqli_fetch_array($email_uzi)) {
            if (strcasecmp($email, $row["email"]) == 0) {
                $error = 2;
                break;
            }
        }
    }
}
//kontrola na spravnost hesla
if ($error == 0) {
    if ($_SESSION["role"] == 1)
        $pass_query = mysqli_query($db, "SELECT heslo FROM ctenar WHERE id='$u_id'");
    else
        $pass_query = mysqli_query($db, "SELECT heslo FROM zamestnanec WHERE id='$u_id'");
    while ($row = mysqli_fetch_array($pass_query)) {
        if (hash("sha512", $heslo) != $row["heslo"])
            $error = 3;
    }
}
//aktualizace
if ($error == 0) {
    if ($_SESSION["role"] == 1)
        $pass_query = mysqli_query($db, "UPDATE ctenar SET email='$email', adresa='$adresa', telefon='$telefon' WHERE id='$u_id'");
    else
        $pass_query = mysqli_query($db, "UPDATE zamestnanec SET email='$email', adresa='$adresa', telefon='$telefon' WHERE id='$u_id'");
    if($pass_query){
        header("Location: index.php?pg=ucet&msg=106");
    } else {
        header("Location: index.php?pg=ucet&msg=99");
    }
} else {
    $error += 19;
    header("Location: index.php?pg=set&what=2&msg=" . $error);
}
?>