<?php

	include_once 'dbh.php';

	if(isset($_POST['submit'])){
		$ingrediente_old = mysqli_real_escape_string($conn, $_POST['ID_Ingrediente_old']);
		$ingrediente_new = mysqli_real_escape_string($conn, $_POST['ID_Ingrediente_new']);
		$nome_new = mysqli_real_escape_string($conn, $_POST['nome_new']);
		$note = mysqli_real_escape_string($conn, $_POST['note']);

		$sql = "UPDATE ingrediente SET ID_Ingrediente='$ingrediente_new', Nome='$nome_new', Note='$note' WHERE ID_Ingrediente='$ingrediente_old';";
		mysqli_query($conn, $sql);

		header("Location: ../php/success.php");
	}
	else{
		header("Location: ../php/error.php");
	}

?>