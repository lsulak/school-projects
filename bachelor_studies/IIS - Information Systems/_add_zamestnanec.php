<?php
/*Skript pro pridani zamestnance do db administratorem*/
session_start();
include "mysql.php";
//pomocne promenne
if ($_SESSION['role'] > 2) {
    //pomocne promenne
    $error = 0;
    $insert = array();
    //zpracovani udaju z formulare
    foreach ($_POST as $key => $value) {
        $value = mysqli_real_escape_string($db, $value);
        switch ($key) {
            case "login":
                if (strlen($value) < 5 || strlen($value) > 32) {
                    $error = 1; // login <5 >32
                    break;
                }
                $used_logins_zam = mysqli_query($db, "SELECT login FROM zamestnanec");
                while ($row = mysqli_fetch_array($used_logins_zam)) {
                    if (strcasecmp($row["login"], $value) == 0) {//vyhledavani loginu v databazi zamestnancu a porovnavani se zadanym loginem
                        $error = 2; // login uz pouzit
                        break;
                    }
                }
                $used_logins_uzi = mysqli_query($db, "SELECT login FROM ctenar");
                while ($row = mysqli_fetch_array($used_logins_uzi)) {
                    if (strcasecmp($row["login"], $value) == 0) {//vyhledavani loginu v databazi uzivatelu a porovnavani se zadanym loginem
                        $error = 2; // login uz pouzit
                        break;
                    }
                }
                $insert[0] = $value;
                break;
            case "heslo":
                if (strlen($value) < 6 || strlen($value) > 32)
                    $error = 3; // heslo <6 >32
                break;
            case "pass_check":
                if ($value != $_POST["heslo"])
                    $error = 4; // neshodujici se heslo
                break;
            case "email":
                $insert[5] = $value;
                if (strlen($value) == 0) {
                    $error = 5;
                    break; // nezadan email
                }
                $used_emails_zam = mysqli_query($db, "SELECT email FROM zamestnanec");
                while ($row = mysqli_fetch_array($used_emails_zam)) {
                    if (strcasecmp($row["email"], $value) == 0) {//vyhledavani loginu v databazi a porovnavani se zadanym loginem
                        $error = 6; // email uz pouzit
                        break;
                    }
                }
                $used_emails_uzi = mysqli_query($db, "SELECT email FROM ctenar");
                while ($row = mysqli_fetch_array($used_emails_uzi)) {
                    if (strcasecmp($row["email"], $value) == 0) {//vyhledavani loginu v databazi a porovnavani se zadanym loginem
                        $error = 6; // email uz pouzit
                        break;
                    }
                }
                break;
            case "jmeno":
                $insert[2] = $value;
                if (strlen($value) == 0)
                    $error = 7;
                break;
            case "prijmeni":
                $insert[3] = $value;
                if (strlen($value) == 0)
                    $error = 8;
                break;
            case "adresa":
                $insert[4] = $value;
                break;
            case "telefon":
                $insert[6] = $value;
                break;
            default:
                break;
        }
        if ($error != 0) break;
    }
    $insert[1] = hash("sha512", $_POST["heslo"]);
    $insert[7] = 0;
    //serazeni pomocneho pole pro snadnejsi vlozeni do databaze
    ksort($insert);
    if ($error == 0) {
        //vytvoreni stringu pro vlozeni do db
        $insert_string = "INSERT INTO zamestnanec (login, heslo, jmeno, prijmeni, adresa, email, telefon, prava)
		VALUES ('" . implode("', '", $insert) . "')";
        //vlozeni uzivatele do db
        if(mysqli_query($db, $insert_string)){
            header("location: index.php?msg=121");
        } else {
            header("location: index.php?msg=99");
        }

    } else //navrat na registraci s chybou
    {
        $error += 4;
        header("location: index.php?pg=errzam&msg=" . $error .
            "&login=" . $_POST["login"] .
            "&email=" . $_POST["email"] .
            "&jmeno=" . $_POST["jmeno"] .
            "&prijmeni=" . $_POST["prijmeni"] .
            "&adresa=" . $_POST["adresa"] .
            "&telefon=" . $_POST["telefon"]);
    }
} else { //ty tu nemas co delat
    header("location: index.php");
}
?>