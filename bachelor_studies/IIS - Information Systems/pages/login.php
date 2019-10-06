<div>
    <? if ($_SESSION["status"] == 0) {
        ?>
        <form action="_login.php" method="post">
            Login: </br>
            <input maxlength="32" id="inputtext2" type="text" name="login" value=""/></br>
            Heslo: </br>
            <input maxlength="32" id="inputtext2" type="password" name="heslo" value=""/></br>
            <input type="hidden" name="page" value='<? echo $redir; ?>'/>
            <input type="submit" value="Přihlásit"/></br>
        </form>
        <? ;
    } else { ?>
        <form action="_logout.php" method="post">
            <input type="hidden" name="page" value='<? echo $redir; ?>'/>
            </br><input type="submit" value="Odhlásit"/>
        </form>
        <? ;
    } ?>
</div>