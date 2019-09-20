<?php

	include_once 'dbh.php';

	if(isset($_POST['submit'])){
		$panino_old = mysqli_real_escape_string($conn, $_POST['ID_Panino_old']);
		$panino_new = mysqli_real_escape_string($conn, $_POST['ID_Panino_new']);
		$nome_new = mysqli_real_escape_string($conn, $_POST['nome_new']);
		$prezzo_new = mysqli_real_escape_string($conn, $_POST['prezzo_new']);

		$sql = "UPDATE panino SET ID_Panino='$panino_new', Nome='$nome_new', Prezzo_EUR='$prezzo_new' WHERE ID_Panino='$panino_old';";
		mysqli_query($conn, $sql);

		header("Location: ../php/success.php");
	}
	else{
		header("Location: ../php/error.php");
	}

?>