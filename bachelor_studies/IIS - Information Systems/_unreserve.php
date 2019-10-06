<?php
/* Skript pro odrezervovani knihy uzivatelem nebo zamestnancem */
session_start();
include 'mysql.php';

if ($_SESSION['role'] > 0) {
    if (!isset($_GET['id']))
        if ($_SESSION['role'] > 1)
            header("Location: index.php?pg=man&co=rezervace&msg=39");
        else
            header("Location: index.php?pg=manage&what=1&msg=39");
    else {
        $r_id = $_GET['id'];
        if ($_SESSION['role'] == 1) { //ctenar
            $u_id = $_SESSION['logged'];
            $rights = mysqli_query($db, "SELECT id_vytisk FROM rezervace WHERE id='$r_id' AND id_ctenar='$u_id'");
            if (mysqli_num_rows($rights) == 0)
                header("Location: index.php?pg=manage&what=1&msg=39");
            $vt = mysqli_fetch_assoc($rights);
            $v_id = $vt['id_vytisk'];
        } else {
            $control = mysqli_query($db, "SELECT id, id_vytisk FROM rezervace WHERE id='$r_id'");
            if (mysqli_num_rows($control) == 0)
                header("Location: index.php?pg=man&co=rezervace&msg=39");
            $vt = mysqli_fetch_assoc($control);
            $v_id = $vt['id_vytisk'];
        }
        if(mysqli_query($db, "DELETE FROM rezervace WHERE id='$r_id'")){
            if(mysqli_query($db, "UPDATE vytisk SET dostupnost='1' WHERE id='$v_id'")){
                if ($_SESSION['role'] == 1)
                    header("Location: index.php?pg=manage&what=1&msg=114");
                else
                    header("Location: index.php?pg=man&co=rezervace&msg=115");
            } else {
                if ($_SESSION['role'] == 1)
                    header("Location: index.php?pg=manage&what=1&msg=99");
                else
                    header("Location: index.php?pg=man&co=rezervace&msg=99");
            }
        } else {
            if ($_SESSION['role'] == 1)
                header("Location: index.php?pg=manage&what=1&msg=99");
            else
                header("Location: index.php?pg=man&co=rezervace&msg=99");
        }



    }
}
?>