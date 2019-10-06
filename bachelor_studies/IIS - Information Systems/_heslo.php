<?php
/*Skript pro zaslani zapomenuteho hesla na zadany email*/
session_start();
include "mysql.php";
//pomocna promenna
$error = 0;
$zam = 1;
//zpracovani udaje z formulare
$value = mysqli_real_escape_string($db, $_POST["email"]);
//vyhledani uzivatelu s timto emailem v db
$uzi = mysqli_query($db, "SELECT * FROM ctenar WHERE email='$value'");
$zam = mysqli_query($db, "SELECT * FROM zamestnanec WHERE email='$value'");
//kontrola
if (strlen($value) == 0) {
    $error = 1;
} else {
    if (mysqli_num_rows($zam) != 1) {
        $zam = 0;
        if (mysqli_num_rows($uzi) != 1) {
            $error = 2;
        }
    }
}
//zpracovani zadosti
if ($error == 0) {    //generovani noveho hesla
    $znaky = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    $newpass = "";
    for ($i = 0; $i < 20; $i++) {
        $newpass .= $znaky[rand(0, strlen($znaky) - 1)]; // generovani noveho hesla
    }
    $newpass_hash = hash("sha512", $newpass);
    if ($zam == 0) { // zmena uzivatelskeho hesla
        while ($row = mysqli_fetch_array($uzi)) {
            $login = $row["login"];
            $jmeno = $row["jmeno"];
            $prijmeni = $row["prijmeni"];
        }
        $upd = mysqli_query($db, "UPDATE ctenar SET heslo='$newpass_hash' WHERE email='$value'");
    } else { //zmena zamestnaneckeho hesla
        while ($row = mysqli_fetch_array($zam)) {
            $login = $row["login"];
            $jmeno = $row["jmeno"];
            $prijmeni = $row["prijmeni"];
        }
        $upd = mysqli_query($db, "UPDATE zamestnanec SET heslo='$newpass_hash' WHERE email='$value'");
    }
    if ($upd) { //zaslani emailu s vygenerovanym heslem
        $mail_text = "<html><body>Vážený uživateli / Vážená uživatelko "
            . $jmeno . " " . $prijmeni . ",<br><br>" .
            "Byla zaznamenána žádost o změnu hesla u účtu " . $login .
            "<br>Bylo vám tudíž vygenerováno nové heslo: <br>"
            . $newpass . "<br>" .
            "Důrazně doporučujeme po přihlášení toto heslo změnit!<br>" .
            "Těšíme se na Vás v naší knihovně!</body></html>";
        $headers = 'MIME-Version: 1.0' . "\r\n";
        $headers .= 'Content-type: text/html; charset=UTF-8' . "\r\n";
        $headers .= 'From: IIS knihovna <knihovna@iis.pesvan.cz>' . "\r\n";
        ini_set('knihovna@iis.pesvan.cz', 'knihovna@iis.pesvan.cz');
        mail($_POST["email"], "Opětovné zaslání hesla do ITI knihovny", $mail_text, $headers);
        header("Location: " . "index.php?pg=msg&msg=104&pg=" . $_POST["page"]);
    } else {
        header("Location: " . "index.php?pg=msg&msg=99&pg=" . $_POST["page"]);
    }
} else {
    $error += 13;
    header("location: index.php?msg=" . $error . "&pg=" . $_POST["page"]);
}
?>