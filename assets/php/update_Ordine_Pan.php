<?php

	include_once 'dbh.php';
	header('Content-type: text/html; charset=UTF-8');
	mysqli_set_charset($conn, "utf8");
	
	if(isset($_POST['submit'])){
		$prenotazione_old = mysqli_real_escape_string($conn, $_POST['ID_Prenotazione_old']);
		$panino_new = mysqli_real_escape_string($conn, $_POST['ID_Panino_new']);
		$quantita_new = mysqli_real_escape_string($conn, $_POST['quantita_new']);
		$note_new = mysqli_real_escape_string($conn, $_POST['note_new']);

		$sql = "UPDATE ordine_pan SET ID_Panino='$panino_new', Quantità=$quantita_new, note='$note_new' WHERE ID_Prenotazione=$prenotazione_old;";
		mysqli_query($conn, $sql);

		header("Location: ../php/success.php");
	}
	else{
		header("Location: ../php/error.php");
	}
?>