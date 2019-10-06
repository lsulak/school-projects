<?php
/*Skript pro registraci*/
session_start();
include "mysql.php";
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
//serazeni pomocneho pole pro snadnejsi vlozeni do databaze
ksort($insert);
if ($error == 0) {
    //vytvoreni stringu pro vlozeni do db
    $insert_string = "INSERT INTO ctenar (login, heslo, jmeno, prijmeni, adresa, email, telefon)
	VALUES ('" . implode("', '", $insert) . "')";
    //vlozeni uzivatele do db
    if(mysqli_query($db, $insert_string)){
        //zaslani informacniho emailu a presmerovani
        $mail_text = "<html><body>Vážený uživateli / Vážená uživatelko, <br><br>" .
            "touto zprávou bychom Vás chtěli přivítat v naší knihovně.
					Nezapomeňte na to, že dokud svou registraci nepotvrdíte osobně,
					 budete mít velmi omezenou možnost rezervace knih přes náš server." .
            "<br>Vaše registrační údaje: <br>" .
            "login: " . $_POST["login"] . "<br>" .
            "email: " . $_POST["email"] . "<br>" .
            "jméno: " . $_POST["jmeno"] . "<br>" .
            "příjmení: " . $_POST["prijmeni"] . "<br>" .
            "adresa: " . $_POST["adresa"] . "<br>" .
            "telefonní číslo: " . $_POST["telefon"] . "<br>" . "<br>" .
            "Login je neměnný, ostatní údaje můžete kdykoliv změnit.<br>" .
            "Těšíme se na Vás v naší knihovně!</body></html>";
        $headers = 'MIME-Version: 1.0' . "\r\n";
        $headers .= 'Content-type: text/html; charset=UTF-8' . "\r\n";
        $headers .= 'From: IIS knihovna <knihovna@iis.pesvan.cz>' . "\r\n";
        ini_set('knihovna@iis.pesvan.cz', 'knihovna@iis.pesvan.cz');
        mail($_POST["email"], "Registrace do ITI knihovny", $mail_text, $headers);
        header("location: index.php?msg=101");
    } else {
        header("location: index.php?msg=99");
    }

} else //navrat na registraci s chybou
{
    $error += 4;
    header("location: index.php?pg=errreg&msg=" . $error .
        "&login=" . $_POST["login"] .
        "&email=" . $_POST["email"] .
        "&jmeno=" . $_POST["jmeno"] .
        "&prijmeni=" . $_POST["prijmeni"] .
        "&adresa=" . $_POST["adresa"] .
        "&telefon=" . $_POST["telefon"]);
}
?>