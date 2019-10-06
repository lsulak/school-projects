<?php
/*Skript pro zmenu hesla */
session_start();
include 'mysql.php';
//pomocna promenna
$error = 0;
//zpracovani formulare
$old = mysqli_real_escape_string($db, $_POST["stare"]);
$new = mysqli_real_escape_string($db, $_POST["nove"]);
$new_c = mysqli_real_escape_string($db, $_POST["nove_check"]);
//zakladni kontroly
if (strlen($new) < 6 || strlen($new) > 32)
    $error = 1; // kratke heslo
elseif (strcmp($new, $new_c) != 0) {
    $error = 2; //hesla se nerovnaji
}
//kontrola stareho hesla
$u_id = $_SESSION["logged"];
if ($_SESSION["role"] == 1)
    $old_query = mysqli_query($db, "SELECT heslo FROM ctenar WHERE id='$u_id'");
else
    $old_query = mysqli_query($db, "SELECT heslo FROM zamestnanec WHERE id='$u_id'");
if ($error == 0)
    while ($row = mysqli_fetch_array($old_query))
        if (hash("sha512", $old) != $row["heslo"])
            $error = 3;
//aktualizace hesla
if ($error == 0) {
    $new = hash("sha512", $new);
    if ($_SESSION["role"] == 1) {
        if (mysqli_query($db, "UPDATE ctenar SET heslo='$new' WHERE id='$u_id'")) {
            header("Location: index.php?pg=ucet&msg=105");
        } else {
            header("Location: index.php?pg=ucet&msg=99");
        }
    } else {
        if (mysqli_query($db, "UPDATE zamestnanec SET heslo='$new' WHERE id='$u_id'")) {
            header("Location: index.php?pg=ucet&msg=105");
        } else {
            header("Location: index.php?pg=ucet&msg=99");
        }
    }
} else {
    $error += 16;
    header("Location: index.php?pg=set&what=1&msg=" . $error);
}


?>