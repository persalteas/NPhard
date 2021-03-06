/* ****************************************************************** */
/*          COLORATION DE GRAPHES                                     */
/*                                                                    */
/*   Louis Becquey                                                    */
/*   Ambre Thomas                                                     */
/*                                 Bioinformatique et modelisation    */
/*                                           INSA Lyon                */
/* ****************************************************************** */

#include "graphe.h"

//======================================================================
//============================== UTILITAIRES ===========================
//======================================================================

void	display_graph(graphe g, int n){
	int i,j;
	printstr("\ngraph:\n");
	for ( i=0; i<n; i++){
		printstr("\n");
		for ( j=0; j<n; j++ ){
			printstr(" ");
			printnbr(g.adj_matrix[i][j]);
		}
	}
	printstr("\n\n");
}

void	display_vecteur(int* vecteur, int n){
	int i;
	printstr("\ncouleurs (sommet: couleur):\n");
	for ( i=0; i<n; i++ ){
		printstr(" ");
		printnbr(i);
		printstr(": ");
		printnbr(vecteur[i]);
		printstr("\n");
	}
	printstr("\n");
}

graphe	graph_from_file(graphe g, const char *input_file){
	int i,j,k;
	int order = -1;
	i = 19;
	while (input_file[i] != '\n'){
		if ((input_file[i] != ',') && (input_file[i] != ' '))
			order++;
		i++;
	}
	i++;
	while (input_file[i] != '\n')
		i++;
	g.order = order;
	for ( j=0; j<order; j++ ){
		for ( k=0; k<order; k++ ){
			g.adj_matrix[j][k] = 0;
		}
	}
	i++;
	while (input_file[i] && (input_file[i] != '.')){
		j = 0;
		k = 0;
		while ((input_file[i] != '\n') && (input_file[i] !='.')){
			if ((input_file[i] != ',') && (input_file[i] != ' ')){
				if (j==0){ j = input_file[i] - 65; }
				else {k = input_file[i] - 65; }
			}
			i++;
		}
		g.adj_matrix[k][j] = 1;
		g.adj_matrix[j][k] = 1;
		if (input_file[i] != '.') i++;
	}
	for ( i=0; i<g.order; i++ )
		g.coloration[i] = 0;
	return (g);
}

//======================================================================
//================= ALGORITHME ZYKOV ===================================
//======================================================================

int		deg_max(int* degree, int n){
	int i, max;

	max = 0;
	for ( i=0; i<n; i++ )
		if (degree[i] > degree[max]) max = i;
	return (max);
}

int		is_voisin_universel(graphe g, int n, int s){
	int i;

	for ( i=0; i<n; i++ )
		if (i!=s && g.adj_matrix[s][i]==0) return (0);
	return (1);
}

int		voisins_en_commun(graphe g, int n, int s, int t){
	int i;
	int k = 0;

	for ( i=0; i<n; i++ )
		if (g.adj_matrix[s][i]==1 && g.adj_matrix[t][i]==1) k++;
	return (k);
}

int		max_voisins_non_voisin(graphe g, int n, int s){
	int i;
	int index_max = n+1; //non trouvé
	int max = 0;
	
	for ( i=0; i<n; i++ ){
		if (s!=i && g.adj_matrix[s][i]==0){ //noeud non voisin de s
			if (index_max > n )
				index_max = i; // récupérer même si pas de voisins en commun
			if (voisins_en_commun(g,n,s,i) > max){
				max = voisins_en_commun(g,n,s,i);
				index_max = i;
			}
		}
	}
	if (index_max < n) return (index_max);
	printstr("ce noeud est voisin de tous les autres !\n");
	return (index_max);
}

void	remove_from_g(int node, graphe g, int n, int* node_index){
	int	i,j;

	printstr("  ( ");
	printnbr(node_index[node]);
	printstr(" est supprime du graphe )\n");
	//remove from node_index
	for ( i=node+1; i<n; i++ )
		node_index[i-1] = node_index[i];
	//remove node (ligne)
	for ( i=node+1; i<n; i++ )
		for ( j=0; j<n; j++ )
			g.adj_matrix[i-1][j] = g.adj_matrix[i][j];
	//remove node (colonne)
	for ( i=node+1; i<n; i++ )
		for ( j=0; j<n; j++ )
			g.adj_matrix[j][i-1] = g.adj_matrix[j][i];
}

int		contract(int from_node, int to_node, graphe g, int n, int* node_index){
	int i;

	printstr("On contracte ");
	printnbr(node_index[from_node]);
	printstr(" en ");
	printnbr(node_index[to_node]);
	printstr("... ");
	//contract en ligne
	for ( i=0; i<n; i++ )
		if (g.adj_matrix[from_node][i]==0 && g.adj_matrix[to_node][i]==1)
			g.adj_matrix[from_node][i] = 1;
	//contract en colonne
	for ( i=0; i<n; i++ )
		if (g.adj_matrix[i][from_node]==0 && g.adj_matrix[i][to_node]==1)
			g.adj_matrix[i][from_node] = 1;
	remove_from_g(from_node, g, n, node_index);
	if (to_node > from_node) to_node--;
	return to_node;
}

void	zykov(graphe g){
	int		degree[g.order];
	int		node_index[g.order];
	int 	couleurs = 0;
	int		i,j,s,t;
	int		n = g.order;

	//init
	for ( i=0; i<g.order; i++ )
		node_index[i] = i;

	//go
	while (n){
		for ( i=0; i<n; i++ ){
			degree[i] = 0;
			for ( j=0; j<n; j++ )
				if (g.adj_matrix[i][j]==1) degree[i]++;
		}
		s = deg_max(degree, n);
		g.coloration[node_index[s]] = ++couleurs;
		printstr("coloration de ");
		printnbr(node_index[s]);
		printstr(" avec la couleur ");
		printnbr(couleurs);
		printstr("\n");
		while (is_voisin_universel(g,n,s)==0){
			t = max_voisins_non_voisin(g,n,s);
			g.coloration[node_index[t]] = couleurs;
			printstr("coloration de ");
			printnbr(node_index[t]);
			printstr(" avec la couleur ");
			printnbr(couleurs);
			printstr("\n");
			s=contract(t, s, g, n, node_index);
			n--;
		}
		remove_from_g(s,g,n,node_index);
		n--;
	}
}

//======================================================================
//================= ALGORITHME MAX_CLIQUE ==============================
//======================================================================

void	remove_from_matrix(int** adj_matrix, int* clique, int len_max_c, int from){
	int**	old_g_adj;
	int		i,j,k,to_keep;
	
	if ((old_g_adj = (int**)malloc(sizeof(int*)*from))==0)
		return ;
	for ( k=0; k<from; k++)
		if ((old_g_adj[k] = (int*)malloc(sizeof(int)*from))==0)
			return ;

	//retirer les lignes
	for ( j=0; j<from; j++ ){
		for ( k=0; k<from; k++ ){
			old_g_adj[j][k] = adj_matrix[j][k];
		}
	}
	k = -1;
	for ( i=0; i<from; i++ ){
		to_keep = 1;
		for ( j=0; j<len_max_c; j++ )
			if ( clique[j] == i )
				to_keep = 0;
		if ( to_keep == 1){
			k++;
			for ( j=0; j<from; j++ )
				adj_matrix[k][j] = old_g_adj[i][j];
		}
	}
	//retirer les colonnes
	for ( j=0; j<from; j++ ){
		for ( k=0; k<from; k++ ){
			old_g_adj[j][k] = adj_matrix[j][k];
		}
	}
	k=-1;
	for ( i=0; i<from; i++ ){
		to_keep = 1;
		for ( j=0; j<len_max_c; j++ )
			if ( clique[j] == i )
				to_keep = 0;
		if ( to_keep == 1){
			k++;
			for ( j=0; j<from; j++ )
				adj_matrix[j][k] = old_g_adj[j][i];
		}
	}

	for ( k=0; k<from; k++ )
		free(old_g_adj[k]);
	free(old_g_adj);
}

int		is_clique(graphe g, int* max_clique, int k, int* combin){
	int i,j;

	for( i=0; i<k; i++ )
		for ( j=0; j<k; j++ )
			if (combin[i]!=combin[j] && g.adj_matrix[max_clique[combin[i]]][max_clique[combin[j]]]==0) return(0);
	printstr("   clique de taille ");
	printnbr(k);
	printstr(" trouvee : {");
	for ( i=0; i<k; i++ ){
		printstr(" ");
		printnbr(max_clique[combin[i]]);
	}
	printstr(" }\n");
	return (1);
}

void	disp_combin(int** combin, int n_combi, int k){
	int i,j;

	i = 0;
	while ( i<n_combi ){
		printstr("   ");
		j = 0;
		while ( j<k ){
			printnbr(combin[i][j]);
			printstr(" ");
			j++;
		}
		printstr("\n");
		i++;
	}
}

void	bruteforce_search(graphe g){
	int n = g.order;
	int couleurs = 0;
	int i,j,k,l,n_combin,croissant;
	int** combin;
	int max_clique[n];  // peut contenir au max n noeuds
	int len_max_c; // toujours <= n
	int degree[n];

	// init
	for (i=0; i<n; i++){
		for (j=0; j<n; j++)
			g.adj_matrix[i][j] = 1 - g.adj_matrix[i][j];
		g.adj_matrix[i][i] = 0;
	}

	//boucle principale
	while (n>0)
	{
		for (i=0; i<n; i++ ){
			k = 0;
			for ( j=0; j<n; j++ )
				k += g.adj_matrix[i][j];
			degree[i] = k;
		}
		printstr("\n==> Encore ");
		printnbr(n);
		printstr(" noeuds a colorier.\nDegres: {");
		for ( i=0; i<n; i++ ){
			printstr(" ");
			printnbr(degree[i]);
		}
		printstr(" }\n");
		//=== recherche de la plus grande clique ===
		len_max_c = 0;
		k = n;
		while ( k>0 && len_max_c == 0 ){
			printstr("Recherche d'une clique de taille ");
			printnbr(k);
			printstr("...\n");
			//chercher si une clique de taille k existe
			l = 0;
			for ( i=0; i<n; i++ ){
				if ( degree[i] >= k-1 ){
					max_clique[l] = i;
					l++;
				}
			}
			//On a l noeuds de degré >=(k-1), rangés dans max_clique
			//s'il y en a moins de k: pas de k-clique possible
			if ( l>=k ){
				printstr("   clique possible. verifions... ");


				//chercher les combinaisons de k noeuds parmi ces l
				n_combin = combinaisons(l, k);
				printnbr(n_combin);
				printstr(" possibilites identifiees\n");
				if ((combin = (int**)malloc(sizeof(int*)*n_combin))==0)
					return;
				for ( i=0; i<n_combin; i++)
					if ((combin[i] = (int*)malloc(sizeof(int)*k))==0)
						return;
				for (i=0; i<n_combin; i++)
					for (j=0; j<k; j++)
						combin[i][j] = 0;
				for ( j=0; j<k; j++ ){
					combin[n_combin-1][j] = j+l-k;
					combin[0][j] = j; // la première combinaison est 0,1,2,...,k-1
				}
				j = 0;
				while (j<(n_combin-1)) // dernière combinaison possible
				{
					i = 1;
					croissant = 1;
					while (i < k) //vérifie l'ordre croissant
					{
						if (combin[j][i - 1] >= combin[j][i])
							croissant = 0;
						i++;
					}
					//~ disp_combin(combin, n_combin, k);
					if (croissant && j==(n_combin-2)) break;
					if (croissant){
						j++;
						i = 0;
						while (i < k) {// recopie la précédente combinaison
							combin[j][i] = combin[j-1][i];
							i++;
						}
					}
					combin[j][k - 1]++; //incrémente la dernière case
					i = k;
					while (i) // si une case a atteint l, on passe �  la case précédente
					{
						i--;
						if (combin[j][i] == l )
						{
							if (i)
								combin[j][i - 1]++;
							combin[j][i] = 0 ;
						}
					}
				}





				//pour chaque combinaison, vérifier si c'est une clique:
				for ( i=0; i<n_combin; i++ ){
					if (is_clique(g, max_clique, k, combin[i])){
						//si oui, récupérer les numéros des noeuds concernés
						for ( j=0; j<k; j++ )
							max_clique[j] = max_clique[combin[i][j]];
						len_max_c = k;
						break;
					}
				}
				
				for ( i=0; i<n_combin; i++ )
					free(combin[i]);
				free(combin);
				combin = 0;
			}
			k--;
		}
		k++;
		//=== colorier la clique trouvée ===
		couleurs++;
		i = 0;
		j = 0;
		l = 0;
		while ( i<len_max_c ){
			if (g.coloration[l] != 0)
				j++;
			if (g.coloration[l] == 0 && l == (max_clique[i]+j)){
				g.coloration[l] = couleurs;
				i++;
			}
			l++;
		}

		//=== soustraire la clique trouvée au graphe g ===
		n -= len_max_c;
		printstr("   je retire ");
		printnbr(k);
		printstr(" noeuds, on aura n=");
		printnbr(n);
		printstr(".\n");
		remove_from_matrix(g.adj_matrix, max_clique, len_max_c, g.order);
		g.order = n;
	}
}

//======================================================================
//================= ALGORITHME GLOUTON =================================
//======================================================================

int*	copyYdansZ(int* destination, int* origine, graphe g){
	int i;
	for ( i=0; i<g.order; i++ )
		destination[i] = origine[i];
	return (destination);
}

int		premier_sommet_de_Z(int* vecteur, graphe g){
	int i;
	for ( i=0; i<g.order; i++ )
		if ( vecteur[i] == 0 )
			return (i);
	return (-1);
}

int*	retirer_s_et_ses_voisins(int s, int* vecteur, graphe g){
	int i;
	vecteur[s] = 1;
	vecteur[g.order + 1]++;
	for ( i=0; i<g.order; i++)
		if ( (g.adj_matrix[s][i] > 0) && (vecteur[i] == 0)){
			vecteur[g.order + 1]++;
			vecteur[i] = 1;
		}
	return (vecteur);
}

void	glouton1(graphe g){
	int i,s;
	int* Z;
	int cardY = 0;
	int color = 0;

	if ((Z = (int *)malloc(sizeof(int)*(g.order+2))) == NULL)
		return;
	Z[g.order+1] = 0; //contient cardZ

	//init:
	for ( i=0; i<g.order; i++ ){
		g.coloration[i] = 0;
		Z[i] = 0;
	}
	while ( cardY < g.order ){
		Z = copyYdansZ(Z, g.coloration, g);
		Z[g.order+1] = cardY;
		color++;
		printstr("\n   Changement de couleur pour ");
		printnbr(color);
		while ( Z[g.order+1] < g.order ){
			s = premier_sommet_de_Z(Z,g);
			printstr("\ncoloration de ");
			printnbr(s);
			printstr(" avec la couleur ");
			printnbr(color);
			g.coloration[s] = color;
			cardY++;
			Z = retirer_s_et_ses_voisins(s,Z,g);
		}
	}
	printstr("\n");
	free(Z);
}

//======================================================================
//=============== ALGORITHME GLOUTON II ================================
//======================================================================


int		comptage(int *boite){
	int nombre;
	
	nombre = 0;
	while (boite[nombre])
		nombre++;
	return (nombre);
}

int		compatible(graphe g, int j, int *boite){
	int i;
	
	i = 0;
	while (boite[i])
	{
		if (g.adj_matrix[boite[i] - 1][j] == 1)
			return (0);
		i++;
	}
	return (1);
}

void	printboites(int ***boites, int i, graphe g){
	int j;
	int k;
	
	j = -1;
	while (++j < g.order)
	{
		k = -1;
		while (++k  < g.order)
		{
			printnbr(boites[i][j][k]);
		}
		printstr("\n");
	}
}

int		meilleure(int ***boites, graphe g){
	int i;
	int temp;
	int j;
	
	i = -1;
	temp = g.order;
	while (++i < g.order)
	{
		j = 0;
		while (boites[i][j][0] != 0)
			j++;
		if (temp > j)
			temp = j;
	}
	i = -1;
	while (++i < g.order)
	{
		j = 0;
		while (boites[i][j][0] != 0)
			j++;
		if (j == temp)
			return (i);
	}
	return (0);
}

void	glouton2(graphe g){
	int i;
	int j;
	int k;
	int nombre_boites;
	int place;
	int nombre_bacterie;
	int ***boites;
	
	//initialise les boites
	i = -1;
	boites = (int***)malloc(sizeof(int**) * g.order);
	while (++i < g.order)
	{
		j = -1;
		boites[i] = (int**)malloc(sizeof(int*) * g.order);
		while (++j < g.order)
		{
			k = -1;
			boites[i][j] = (int*)malloc(sizeof(int) * g.order);
			while (++k < g.order)
			{
				boites[i][j][k] = 0;
			}
		}
	}
	//boucle principale
	i = -1;
	while (++i < g.order)
	{
		nombre_boites = 0;
		boites[i][nombre_boites][0] = i + 1;
		j = i + 1;
		if (j == g.order)
			j = 0;
		while (j != i && j != g.order)
		{
			k = -1;
			place = 0;
			while (k++ < nombre_boites && place == 0)
			{
				nombre_bacterie = comptage(boites[i][k]);
				if (compatible(g, j, boites[i][k]))
				{
					boites[i][k][nombre_bacterie] = j + 1;
					place = 1;
				}
			}
			if (place == 0)
			{
				nombre_boites++;
				boites[i][k][0] = j + 1;
			}
			if (i == 0 && j + 1 == g.order)
				break;
			if (j + 1 == g.order)
				j = -1;
			j++;
		}
	}
	//analyse du tableau 3D "boites"
	nombre_boites = meilleure(boites, g);
	//coloration de la meilleure solution
	i = -1;
	while (++i < g.order)
		g.coloration[i] = 0;
	i = -1;
	while (++i < g.order)
	{
		j = -1;
		while (++j < g.order)
		{
			if (boites[nombre_boites][i][j] !=0)
				g.coloration[boites[nombre_boites][i][j] - 1] = i +1;
		}
	}
	i = -1;
	while (++i < g.order)
	{
		j = -1;
		while (++j < g.order)
			free(boites[i][j]);
		free(boites[i]);
	}
	free(boites);
}

//======================================================================
//=============== ALGORITHME DSATUR ====================================
//======================================================================

int dsat[ORDER_MAX];
int degre[ORDER_MAX];

void	init_dsat(graphe g){
	int i,j;
	for ( i=0; i<g.order; i++ ){
		dsat[i] = 1; //on commence �  1 pour ne pas avoir de dsat nul si noeud non adjacent �  un autre noeud
		g.coloration[i] = 0;
		for ( j=0; j<g.order; j++ ){
			if ( g.adj_matrix[i][j] ) dsat[i]++;
		}
	}
	//ici, dsat vaut degré + 1
}

int		sommet_non_colorie_de_dsat_max(graphe g){
	int v,i,dsat_max_local = 0;
	for ( i=0; i<g.order; i++ ){
		if (g.coloration[i] == 0){
			if (dsat[i] > dsat_max_local){
				v = i;
				dsat_max_local = dsat[i];
			}
		}
	}
	return (v);
}

int		coloration_avec_couleur_minimum(int t, graphe g){
	int i;
	int color_used[ORDER_MAX];
	//init:
	for ( i=0; i<g.order; i++ )
		color_used[i] = 0;
	for ( i=0; i<g.order; i++ ){
		if ( (g.adj_matrix[t][i]==1) && (g.coloration[i] > 0)){
			color_used[g.coloration[i]-1] = 1;
		}
	}
	//on colorie t par la plus petite couleur non utilisée au voisinnage:
	for ( i=0; i<g.order; i++){
		if ( color_used[i] == 0 ){
			return (i);
		}
	}
	return (0);
}

int	 	nb_de_couleurs_differentes_autour_de(int t, graphe g){
	int i, nb_de_couleurs_utilisees_autour_de_t;
	int color_used[ORDER_MAX];
	//init:
	for ( i=0; i<g.order; i++ )
		color_used[i] = 0;
	for ( i=0; i<g.order; i++ ){
		if ( (g.adj_matrix[t][i]) && (g.coloration[i]>0)){
			color_used[g.coloration[i]-1] = 1;
		}
	}
	nb_de_couleurs_utilisees_autour_de_t = 0;
	for ( i=0; i<g.order; i++ )
		if (color_used[i] == 1) nb_de_couleurs_utilisees_autour_de_t++;
	return (nb_de_couleurs_utilisees_autour_de_t);
}

void	maj_du_vecteur_dsat(int t, graphe g){
	int i;
	//synthese pour que le sommet actuellement colorié soit
	//rapidement ecartee pour les autres choix
	dsat[t] = -1;
	for ( i=0; i<g.order; i++ ){
		if ( (g.adj_matrix[t][i]==1) && (g.coloration[i]==0)){
			dsat[i] = nb_de_couleurs_differentes_autour_de(i,g);
		}
	}
}

void	dsatur(graphe g){
	int i,s;
	init_dsat(g);
	for ( i=0; i<g.order; i++ ){
		s = sommet_non_colorie_de_dsat_max(g);
		g.coloration[s] = coloration_avec_couleur_minimum(s,g)+1;
		printstr("\n n");
		printnbr(s);
		printstr(" colorie par ");
		printnbr(g.coloration[s]);
		maj_du_vecteur_dsat(s,g);
	}
	printstr("\n");
}
