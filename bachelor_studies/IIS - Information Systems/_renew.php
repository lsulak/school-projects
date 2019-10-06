<?php
/*Skript pro prodlouzeni vypujcky uzivateli zamestnancem nebo uzivatelem 1x*/
session_start();
include 'mysql.php';

if ($_SESSION['role'] > 0) {
    $p_id = $_GET['id'];
    $pred = mysqli_query($db, "SELECT datum_vraceni FROM vypujcka WHERE id='$p_id'");
    $predc = mysqli_fetch_assoc($pred);
    $pre_vr = $predc['datum_vraceni'];
    if ($_SESSION['role'] == 1) {
        $u_id = $_SESSION['logged'];
        $cont = mysqli_query($db, "SELECT id FROM vypujcka WHERE id='$p_id' AND id_ctenar='$u_id' AND u_prodl='0'");
        if (mysqli_num_rows($cont) == 0) {
            $error = 1;
            header("location: index.php?pg=show&what=1&msg=44");
        }
    }
    //pokud je novy mesic leden, je potreba se presunout do dalsiho roku
    if ($error != 1) {
        if (strlen($pre_vr) == 7)
            $pre_vr = "0" . $pre_vr;
        $d = intval(substr($pre_vr, 0, 2));
        $m = intval(substr($pre_vr, 2, 2));
        $m++;
        if ($m == '13')
            $m = "01";
        if (strlen($m) == 1)
            $m = "0" . $m;
        $y = intval(substr($pre_vr, 4, 4));
        if ($m == "01")
            $y++;
        $dat_vr = $d . $m . $y;
        if ($_SESSION['role'] == 1) {
            if(mysqli_query($db, "UPDATE vypujcka SET datum_vraceni='$dat_vr',u_prodl='1' WHERE id='$p_id'")){
                header("location: index.php?pg=show&what=1&msg=119");
            } else {
                header("location: index.php?pg=show&what=1&msg=99");
            }
        } else {
            if(mysqli_query($db, "UPDATE vypujcka SET datum_vraceni='$dat_vr' WHERE id='$p_id'")){
                header("location: index.php?pg=man&co=vypujcka&msg=119");
            } else {
                header("location: index.php?pg=man&co=vypujcka&msg=99");
            }
        }
    }
} else {
    header("location: index.php");
}
?>