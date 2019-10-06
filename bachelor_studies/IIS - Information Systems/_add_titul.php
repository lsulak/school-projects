<?php
/*Skript pro pridani titulu do databaze */
session_start();
include "mysql.php";
//pomocne promenne
if ($_SESSION['role'] > 1) {
    $error = 0;
    //zpracovani udaju z formulare
    $nazev = mysqli_real_escape_string($db, $_POST["nazev"]);
    if (strlen($nazev) == 0)
        $error = 27; //nezadano
    $autor = mysqli_real_escape_string($db, $_POST["autor"]);
    if (strlen($autor) == 0)
        $error = 28; //nezadano
    else {
        $isright = strpos($autor, ' ');
        if ($isright == FALSE)
            $error = 32;
    }

    $vydani = mysqli_real_escape_string($db, $_POST["vydani"]);
    if (strlen($vydani) == 0)
        $error = 29; //nezadano
    if (strlen($vydani) > 4 || !is_numeric($vydani))
        $error = 30; //spatny format data

    $isbn = mysqli_real_escape_string($db, $_POST["isbn"]);
    if (strlen($isbn) == 0)
        $isbn = 0;

    $nakladatelstvi = mysqli_real_escape_string($db, $_POST["nakladatelstvi"]);
    if (strlen($nakladatelstvi) == 0)
        $nakladatelstvi = 0;
    //vlozeni
    $edit = $_POST['edit'];
    if ($error == 0) {
        $autori = explode(",", $autor);
        foreach ($autori as $key => $jeden_autor) {
            $tmp = strrpos($jeden_autor, ' ');
            $jmeno = substr($jeden_autor, 0, $tmp);
            $prijmeni = substr($jeden_autor, $tmp + 1);
            $id_t = mysqli_query($db, "SELECT id FROM titul WHERE nazev='$nazev'");
            $autor_id = mysqli_query($db, "SELECT id FROM autor WHERE jmeno='$jmeno' AND prijmeni='$prijmeni'");
            if (mysqli_num_rows($autor_id) == 0) {
                $error = 33;
            }
        }
        if ($error==0) {
            if ($edit == 0) {
                if (!mysqli_query($db, "INSERT INTO titul (nazev, rok_vydani, isbn, nakladatelstvi) VALUES
                    ('$nazev','$vydani','$isbn','$nakladatelstvi')")
                ) {
                    header("location: index.php?pg=add&co=titul&msg=99&nazev=" . $_POST["nazev"] .
                        "&autor=" . $_POST["autor"] .
                        "&vydani=" . $_POST["vydani"] .
                        "&isbn=" . $_POST["isbn"] .
                        "&nakladatelstvi=" . $_POST["nakladatelstvi"]);
                }
                foreach ($autori as $key => $jeden_autor) {
                    $tmp = strrpos($jeden_autor, ' ');
                    $jmeno = substr($jeden_autor, 0, $tmp);
                    $prijmeni = substr($jeden_autor, $tmp + 1);

                    $id_t = mysqli_query($db, "SELECT id FROM titul WHERE nazev='$nazev'");

                    while ($row = mysqli_fetch_array($id_t)) {
                        $t_id = $row["id"];
                    }

                    $autor_id = mysqli_query($db, "SELECT id FROM autor WHERE jmeno='$jmeno' AND prijmeni='$prijmeni'");
                    if (mysqli_num_rows($autor_id) != 0) {
                        while ($row2 = mysqli_fetch_array($autor_id)) {
                            $a_id = $row2["id"];
                            if (!mysqli_query($db, "INSERT INTO autor_titul (id_autor, id_titul) VALUES ('$a_id', '$t_id')")) {
                                $error = 33;
                                if (mysqli_query($db, "DELETE FROM titul WHERE id='$t_id'")) {
                                    header("location: index.php?pg=add&co=titul&msg=" . $error .
                                        "&nazev=" . $_POST["nazev"] .
                                        "&autor=" . $_POST["autor"] .
                                        "&vydani=" . $_POST["vydani"] .
                                        "&isbn=" . $_POST["isbn"] .
                                        "&nakladatelstvi=" . $_POST["nakladatelstvi"]);
                                } else {
                                    header("location: index.php?pg=add&co=titul&msg=99&nazev=" . $_POST["nazev"] .
                                        "&autor=" . $_POST["autor"] .
                                        "&vydani=" . $_POST["vydani"] .
                                        "&isbn=" . $_POST["isbn"] .
                                        "&nakladatelstvi=" . $_POST["nakladatelstvi"]);
                                }
                            }
                        }
                    } else {
                        $error = 33;
                        if (mysqli_query($db, "DELETE FROM titul WHERE id='$t_id'")) {
                            header("location: index.php?pg=add&co=titul&msg=" . $error .
                                "&nazev=" . $_POST["nazev"] .
                                "&autor=" . $_POST["autor"] .
                                "&vydani=" . $_POST["vydani"] .
                                "&isbn=" . $_POST["isbn"] .
                                "&nakladatelstvi=" . $_POST["nakladatelstvi"]);
                        } else {
                            header("location: index.php?pg=add&co=titul&msg=99&nazev=" . $_POST["nazev"] .
                                "&autor=" . $_POST["autor"] .
                                "&vydani=" . $_POST["vydani"] .
                                "&isbn=" . $_POST["isbn"] .
                                "&nakladatelstvi=" . $_POST["nakladatelstvi"]);
                        }

                    }
                }

                //presmerovani
                if ($error == 0)
                    header("location: index.php?pg=tituly&msg=108");
            } else { //modifikace
                $e_id = $_POST['id'];
                $autori = explode(",", $autor);
                $a_ids = array();
                foreach ($autori as $key => $jeden_autor) {
                    $tmp = strrpos($jeden_autor, ' ');
                    $jmeno = substr($jeden_autor, 0, $tmp);
                    $prijmeni = substr($jeden_autor, $tmp + 1);
                    $autor_id = mysqli_query($db, "SELECT id FROM autor WHERE jmeno='$jmeno' AND prijmeni='$prijmeni'");
                    //pridat autory kteri pribyli
                    if (mysqli_num_rows($autor_id) != 0) {
                        while ($row2 = mysqli_fetch_array($autor_id)) {
                            $a_id = $row2["id"];
                            array_push($a_ids, $a_id);
                            $cont = mysqli_query($db, "SELECT * FROM autor_titul WHERE id_autor='$a_id' AND id_titul='$e_id'");
                            if (mysqli_num_rows($cont) == 0) //pokud se zaznam v tabulce nenachazi
                                if (!mysqli_query($db, "INSERT INTO autor_titul (id_autor, id_titul)	VALUES ('$a_id','$e_id')")) {
                                    header("location: index.php?pg=edit&co=titul&msg=99&id=" . $e_id);
                                }
                        }
                    } else {
                        $error = 33;
                        echo 33;
                        header("location: index.php?pg=edit&co=titul&msg=" . $error . "&id=" . $e_id);
                    }
                } //odstraneni nyni prebytecnych autoru
                if ($error == 0) {
                    $del_str = "DELETE FROM autor_titul WHERE id_titul='$e_id' AND id_autor NOT IN ('";
                    $del_str .= implode("', '", $a_ids);
                    $del_str .= "')";
                    //update az na konci
                    if (mysqli_query($db, $del_str)) {
                        $pom = mysqli_query($db, "UPDATE titul SET nazev='$nazev', rok_vydani='$vydani', isbn='$isbn',
                        nakladatelstvi='$nakladatelstvi' WHERE id='$e_id'");
                        if ($pom) {
                            header("location: index.php?pg=tituly&msg=117");
                        } else {
                            header("location: index.php?pg=tituly&msg=99");
                        }
                    } else {
                        header("location: index.php?pg=tituly&msg=99");
                    }
                }
            }
        }
    }
    if($error!=0) //navrat na vytvoreni autora s chybou
    {
        if ($edit == 0)
            header("location: index.php?pg=add&co=titul&msg=" . $error .
                "&nazev=" . $_POST["nazev"] .
                "&autor=" . $_POST["autor"] .
                "&vydani=" . $_POST["vydani"] .
                "&isbn=" . $_POST["isbn"] .
                "&nakladatelstvi=" . $_POST["nakladatelstvi"]);
        else
            header("location: index.php?pg=edit&co=titul&id=" . $_POST['id'] . "&msg=" . $error);
    }
} else { //ty tu nemas co delat
    header("location: index.php");
}
?>