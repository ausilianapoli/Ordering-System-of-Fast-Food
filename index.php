<!DOCTYPE html>
<html lang=ita>
<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<link rel="stylesheet" type="text/css" href="assets/css/style.css">
	<title>La mia Panineria</title>
</head>
<body>
	<h1>Benvenuto</h1>
	<div id=introduction>
		<p>
			Questo è il sistema di gestione digitale delle prenotazioni della tua panineria.<br>
			Il database è così strutturato:
			<ul>
				<li><b>INGREDIENTE</b>(<ins>ID_Ingrediente</ins>, nome, note)</li>
				<li><b>COMPOSIZIONE</b>(<ins>ID_Panino, ID_Ingrediente</ins>)</li>
				<li><b>PANINO</b>(<ins>ID_Panino</ins>, nome, prezzo)</li>
				<li><b>BEVANDA</b>(<ins>ID_Bevanda</ins>, nome, prezzo)</li>
				<li><b>TAVOLO</b>(<ins>ID_Tavolo</ins>)</li>
				<li><b>PRENOTAZIONE</b>(<ins>ID_Prenotazione</ins>, ID_Tavolo, Data/Ora, tot_panini, tot_bevande, Totale_EUR)</li>
				<li><b>ORDINE_BEV</b>(<ins>ID_Prenotazione, ID_Bevanda</ins>, Quantità)</li>
				<li><b>ORDINE_PAN</b>(<ins>ID_Prenotazione, ID_Panino</ins>, Quantità, note)</li>
			</ul>
			Le operazioni consentite sono quelle di <i>inserimento</i>, <i>eliminazione</i>, <i>modifica</i> e <i>visualizzazione</i> delle informazioni.
		</p>
	</div>
	<div class=operations>
		<p id=insert>
			<h3>Inserisci</h3>
			<div class="vertical-menu">
			  <a href="assets/php/insert_form_Tavolo.php">nuovo tavolo</a>
			  <a href="assets/php/insert_form_Bevanda.php">nuova bevanda</a>
			  <a href="assets/php/insert_form_Panino.php">nuovo panino</a>
			  <a href="assets/php/insert_form_Ingrediente.php">nuovo ingrediente</a>
			  <a href="assets/php/insert_form_Composizione.php">nuova composizione</a>
			  <a href="assets/php/insert_form_Ordine_Bev.php">nuovo ordine bevanda</a>
			  <a href="assets/php/insert_form_Ordine_Pan.php">nuovo ordine panino</a>
			  <a href="assets/php/insert_form_Prenotazione.php">nuova prenotazione</a>
			</div>
		</p>
		<p id=delete>
			<h3>Elimina</h3>
			<div class="vertical-menu">
			  <a href="assets/php/delete_form_Tavolo.php">tavolo</a>
			  <a href="assets/php/delete_form_Bevanda.php">bevanda</a>
			  <a href="assets/php/delete_form_Panino.php">panino</a>
			  <a href="assets/php/delete_form_Ingrediente.php">ingrediente</a>
			  <a href="assets/php/delete_form_Composizione.php">composizione</a>
			  <a href="assets/php/delete_form_Ordine_Bev.php">ordine bevanda</a>
			  <a href="assets/php/delete_form_Ordine_Pan.php">ordine panino</a>
			  <a href="assets/php/delete_form_Prenotazione.php">prenotazione</a>
			</div>
		</p>
		<p id=update>
			<h3>Modifica</h3>
			<div class="vertical-menu">
			  <a href="assets/php/update_form_Tavolo.php">tavolo</a>
			  <a href="assets/php/update_form_Bevanda.php">bevanda</a>
			  <a href="assets/php/update_form_Panino.php">panino</a>
			  <a href="assets/php/update_form_Ingrediente.php">ingrediente</a>
			  <a href="assets/php/update_form_Ordine_Bev.php">ordine bevanda</a>
			  <a href="assets/php/update_form_Ordine_Pan.php">ordine panino</a>
			  <a href="assets/php/update_form_Prenotazione.php">prenotazione</a>
			</div>
		</p>
		<p id=select>
			<h3>Visualizza</h3>
			<div class="vertical-menu">
			  <a href="assets/php/select_Tavolo.php">elenco tavoli</a>
			  <a href="assets/php/select_Bevanda.php">elenco bevande</a>
			  <a href="assets/php/select_Panino.php">elenco panino</a>
			  <a href="assets/php/select_Ingrediente.php">elenco ingrediente</a>
			  <a href="assets/php/select_Composizione.php">elenco composizione</a>
			  <a href="assets/php/select_Ordine_Bev.php">elenco ordine bevande</a>
			  <a href="assets/php/select_Ordine_Pan.php">elenco ordine panini</a>
			  <a href="assets/php/select_Prenotazione.php">elenco prenotazioni</a>
			</div>
		</p>
	</div>

</body>
</html>

