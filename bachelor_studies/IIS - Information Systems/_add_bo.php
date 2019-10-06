<?
/* Skript pro vytvoreni simulovane vypujcky*/
session_start();
include 'mysql.php';
if ($_SESSION['role'] > 1) {
    $errpoz = 0;
    $error = 0;
    $pocet = count($_POST);
    for ($a = 0; $a < $pocet; $a++) {
        $pom = $_POST[$a];
        $control = mysqli_query($db, "SELECT id_titul FROM vytisk WHERE id='$pom'");
        if (mysqli_num_rows($control) == 0) {
            $errpoz = $a;
            $error = 1;
            break;
        }
    }
    if ($error == 1) {
        $headerstr = "location: index.php?pg=add&co=vypujcka2&msg=42&errpoz=" . $errpoz . "&id=" . $_GET['id'] . "&pocet=" . $pocet;
        for ($a = 0; $a < $pocet; $a++)
            $headerstr .= "&" . $a . "=" . $_POST[$a];
        header($headerstr);
    } else {
        $u_id = $_GET['id'];
        $m = date("m");
        $m++;
        if (strlen($m) == 1)
            $m = "0" . $m; //pridani 0 na zacatek mesice pokud ma jen jedno cislo
        $y = date("Y");
        if ($m == "01")
            $y = $y++; //pokud je novy mesic leden, je potreba se presunout do dalsiho roku
        $dat_vr = date('d') . $m . $y;
        $cont = mysqli_num_rows(mysqli_query($db, "SELECT jmeno FROM ctenar WHERE id='$u_id'"));
        if ($cont == 0) {
            header("location: index.php?pg=add&co=vypujcka&msg=40");
        }
        for ($a = 0; $a < $pocet; $a++) {
            $v_id = $_POST[$a];
            $dost = mysqli_num_rows(mysqli_query($db, "SELECT * FROM vytisk WHERE dostupnost='1' AND id='$v_id'"));
            if ($dost == 0) { //pokud je nedostupny
                $error = 1;
                $errpoz = $a;
                break;
            }
        }
        if ($error == 0) {
            for ($a = 0; $a < $pocet; $a++) {
                $v_id = $_POST[$a];
                if(!mysqli_query($db, "UPDATE vytisk SET dostupnost='0' WHERE id='$v_id'")){
                    header("location: index.php?msg=99");
                }
                if(!mysqli_query($db, "INSERT INTO vypujcka (id_ctenar, id_vytisk, datum_vraceni) VALUES ('$u_id','$v_id','$dat_vr')")){
                    header("location: index.php?msg=99");
                }
            }
            header("location: index.php?msg=118");
        } else {
            $headerstr = "location: index.php?pg=add&co=vypujcka2&msg=43&errpoz=" . $errpoz . "&id=" . $_GET['id'] . "&pocet=" . $pocet;
            for ($b = 0; $b < $pocet; $b++)
                $headerstr .= "&" . $b . "=" . $_POST[$b];
            header($headerstr);
        }


    }
} else { //nepovolany
    header("location: index.php");
}
?>