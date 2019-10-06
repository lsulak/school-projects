<?php
/* Jednoduchy skript pro nastaveni priznaku pro razeni */
session_start();
if (isset($_POST['autor']))
    header("Location: index.php?pg=tituly&id=" . $_POST["autor"] . "&attr=" . $_POST["attr"] . "&desc=" . $_POST["desc"]);
else
    header("Location: index.php?pg=tituly&attr=" . $_POST["attr"] . "&desc=" . $_POST["desc"]);
?>