<!DOCTYPE html>
<html lang=ita>
<head><meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<link rel="stylesheet" type="text/css" href="../css/style.css">
	<title>La mia Panineria</title>
</head>
<body>
	<h2>Inserisci un nuovo ordine di panino</h2>
	<form action="insert_Ordine_Pan.php" method="POST">
		<input type="number" name="ID_Prenotazione" placeholder="ID prenotazione - Es. 1">
		<input type="text" name="ID_Panino" placeholder="ID panino - Es. P01">
		<input type="number" name="Quantita" placeholder="Quantita' - Es. 1" default="1">
		<input type="text" name="Note" placeholder="Note - Es. senza pomodoro">
		<button type="submit" name="submit">Inserisci</button>
	</form>
	<p></p>
	<div class="vertical-menu">
		<a href="../../index.php" class=active>Torna alla Home</a>
	</div>
</body>
</html>