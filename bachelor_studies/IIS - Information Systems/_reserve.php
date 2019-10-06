<?php
/*Skript pro rezervaci knihy uzivatelem*/
session_start();
include "mysql.php";
$error = 0;
if ($_SESSION['role'] == 1) {
    if (!isset($_GET['id']))
        $error = 1;
    $t_id = $_GET['id'];
    $u_id = $_SESSION['logged'];

    $ins_vyt = mysqli_query($db, "SELECT id FROM vytisk WHERE id_titul='$t_id' AND dostupnost='1' LIMIT 1");
    $cont = mysqli_query($db, "SELECT id FROM rezervace WHERE id_titul='$t_id' AND id_ctenar='$u_id'");
    if (mysqli_num_rows($ins_vyt) == 0 || mysqli_num_rows($cont) > 0)
        $error = 1; //titul neni dostupny nebo jiz jej ma ctenar rezervovan
    $vt = mysqli_fetch_assoc($ins_vyt);
    $v_id = $vt['id'];
    if ($error == 0) {//uprava vytisku
        if(mysqli_query($db, "UPDATE vytisk SET dostupnost='0' WHERE id='$v_id'")){
            //vlozeni zatim neschvalene rezervace
            if(mysqli_query($db, "INSERT INTO rezervace (id_ctenar, stav, id_titul, id_vytisk) VALUES ('$u_id','0','$t_id','$v_id')")){
                header("location: index.php?pg=tituly&msg=112");
            } else {
                header("location: index.php?pg=tituly&msg=99");
            }
        } else {
            header("location: index.php?pg=tituly&msg=99");
        }
    } else //navrat na vytvoreni autora s chybou
    {
        header("location: index.php?pg=tituly&msg=36");
    }
} else { //ty tu nemas co delat
    header("location: index.php?pg=tituly&msg=37");
}
?>