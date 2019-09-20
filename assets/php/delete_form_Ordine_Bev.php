<!DOCTYPE html>
<html lang=ita>
<head><meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<link rel="stylesheet" type="text/css" href="../css/style.css">
	<title>La mia Panineria</title>
</head>
<body>
	<h2>Elimina un ordine di bevande</h2>
	<form action="delete_Ordine_Bev.php" method="POST">
		<input type="number" name="ID_Prenotazione" placeholder="ID prenotazione - Es. 1">
		<input type="text" name="ID_Bevanda" placeholder="ID bevanda - Es. B01">
		<button type="submit" name="submit">Elimina</button>
	</form>
	<p></p>
	<div class="vertical-menu">
		<a href="../../index.php" class=active>Torna alla Home</a>
	</div>
</body>
</html>