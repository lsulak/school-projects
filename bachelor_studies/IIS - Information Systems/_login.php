<?php
/*Skript pro prihlaseni uzivatele*/
session_start();
include 'mysql.php';
//zpracovani udaju z formulare
$login = mysqli_real_escape_string($db, $_POST["login"]);
$heslo = hash("sha512", mysqli_real_escape_string($db, $_POST["heslo"]));
//cteni z tabulek relevantnich pro login
$queryu = mysqli_query($db, "SELECT id, login, heslo FROM ctenar");
$queryz = mysqli_query($db, "SELECT id, login, heslo, prava FROM zamestnanec");
//pomocna promenna
$role = 0;
$u_id = 0;
//hledani v zamestancich, identifikace administratora
while ($row = mysqli_fetch_array($queryz)) {
    if (strcasecmp($row["login"], $login) == 0) {
        if ($row["heslo"] == $heslo) {
            $role = 2; //zamestnanec
            if ($row["prava"] == "1") {
                $role = 3; // administratorska prava
            }
            $u_id = $row["id"];
            break;
        }
    }
}
// pokud se nenajde, analogicky hledani v uzivatelich
if ($role == 0) {
    while ($row = mysqli_fetch_array($queryu)) {
        if (strcasecmp($row["login"], $login) == 0) {
            if ($row["heslo"] == $heslo) {
                $role = 1; //uzivatel
                $u_id = $row["id"];
                break;
            }
        }
    }
}
//konecne zpracovani vysledku prihlaseni
if ($role > 0) {
    $_SESSION["logged"] = $u_id;
    $_SESSION["role"] = $role;
    header("location: index.php?msg=102&pg=" . $_POST["page"]);
} else {
    $_SESSION["logged"] = 0;
    header("location: index.php?msg=13&pg=" . $_POST["page"]);
}
?>