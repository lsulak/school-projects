<div>
    <h2>Registrace</h2>

    <form id="reg" action="_reg.php" method="post">
        *Login (5-32 znaků): <br>
        <input maxlength="32" id="inputtext2" type="text" name="login" value='<? echo $_GET["login"]; ?>'/><br>
        *Heslo (6-32 znaků): <br>
        <input maxlength="32" id="inputtext2" type="password" name="heslo" value=''/><br>
        *Potvrzení hesla: <br>
        <input maxlength="33" id="inputtext2" type="password" name="pass_check" value=''/><br>
        *Email: <br>
        <input maxlength="50" id="inputtext2" type="text" name="email" value='<? echo $_GET["email"]; ?>'/><br>
        *Jméno: <br>
        <input maxlength="50" id="inputtext2" type="text" name="jmeno" value='<? echo $_GET["jmeno"]; ?>'/><br>
        *Příjmení: <br>
        <input maxlength="50" id="inputtext2" type="text" name="prijmeni" value='<? echo $_GET["prijmeni"]; ?>'/><br>
        Adresa: <br>
        <input maxlength="200" id="inputtext2" type="text" name="adresa" value='<? echo $_GET["adresa"]; ?>'/><br>
        Telefon: <br>
        <input maxlength="14" id="inputtext2" type="text" name="telefon" value='<? echo $_GET["telefon"]; ?>'/><br>
        * jsou označeny povinné položky<br><br>
        Na zadaný email Vám přijde potvrzení o registraci<br><br>
        <input type="submit" value="Registrovat"/><br>
    </form>
</div>