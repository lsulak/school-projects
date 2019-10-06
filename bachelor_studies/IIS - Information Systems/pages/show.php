<div>
    <? if ($_SESSION['role'] == 1) {
        if ($_GET['what'] == "1") {
            ?>
            <div class="caption">Aktualní výpůjčky</div>
            <div id="table">
                <div class="header-row row">
                    <span class="cell primary">Jméno</span>
                    <span class="cell">Čas</span>
                    <span class="cell">Titul</span>
                    <span class="cell">ID výtisku</span>
                    <span class="cell">Vrácení do:</span>
                    <span class="cell">Stav</span>
                    <span class="cell">Prodloužit</span>
                </div>
                <?
                $u_id = $_SESSION['logged'];
                $vypujcka = mysqli_query($db, "SELECT * FROM vypujcka WHERE id_ctenar='$u_id' ORDER BY id");
                while ($row = mysqli_fetch_array($vypujcka)) {
                    $v_id = $row["id_vytisk"];
                    $c_id = $row["id_ctenar"];
                    $vytisk = mysqli_query($db, "SELECT id_titul FROM vytisk WHERE id='$v_id'");
                    $row_v = mysqli_fetch_assoc($vytisk);
                    $t_id = $row_v["id_titul"];
                    $titul = mysqli_query($db, "SELECT id, nazev, rok_vydani FROM titul WHERE id='$t_id'");
                    $row_t = mysqli_fetch_assoc($titul);
                    $ctenar = mysqli_query($db, "SELECT id, prijmeni, jmeno FROM ctenar WHERE id='$c_id'");
                    $row_c = mysqli_fetch_assoc($ctenar);
                    $vrac = $row['datum_vraceni'];
                    if (strlen($vrac) == 7)
                        $vrac = "0" . $vrac;
                    if ($row['stav'] == '1')
                        $stav = "V pořádku";
                    else
                        $stav = "Po termínu";
                    $vrac = substr($vrac, 0, 2) . "." . substr($vrac, 2, 2) . "." . substr($vrac, 4);
                    ?>
                    <div class="row">
                        <span class="cell primary"
                              data-label="jmeno"><? echo $row_c['prijmeni'] . ", " . $row_c['jmeno']; ?></span>
                        <tr><span class="cell" data-label="cas"><? echo $row['datum_vypujcky']; ?></span>
						<span class='cell'
                              data-label='titul'><? echo "<a href='index.php?titul=" . $row_t["id"] . "'>" .
                                $row_t["nazev"] . "</a>"; ?></span>
                            <span class='cell' data-label='Zamitnout'><? echo $row['id_vytisk']; ?></span>
                        </tr>
                        <span class='cell' data-label='Potvrdit'><? echo $vrac; ?></span>
                        <span class='cell' data-label=''><? echo $stav; ?></span>
					<span class='cell' data-label=''><?
                        if ($row['u_prodl'] == '0') {
                            ?>
                            <a href="_renew.php?id=<? echo $row['id']; ?>">Prodloužit</a>
                        <? } else { ?>
                            <b>Již prodlouženo</b>
                        <? } ?>
                        <br>
						</span>
                    </div>
                <? } ?>
            </div>
            <? ;
        } ?>
        <? ;
    } ?>
</div>