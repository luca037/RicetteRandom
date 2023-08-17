package main

import (
	"fmt"

	"github.com/luca037/RicetteRandom/GiallozafferanoUtils"
	"golang.org/x/net/html"
)

const URL = "https://www.giallozafferano.it/ricette-cat/Primi/pasta/"

func main() {
	// salvo i link relativi alle pagine (pagine e ricette) in due channels
	pagesUrls := make(chan string, 50)
	pagesUrls <- URL
	pageHeads := make(chan *html.Node, len(pagesUrls))
	recipesUrls := make(chan string, 15*len(pagesUrls))
	recipesHeads := make(chan *html.Node, len(recipesUrls))
	recipesToSave := make(chan Recipe, len(recipesUrls))
	// run
	go MkeRequest(pagesUrls, pageHeads)
	go ParseRecipesPage(pageHeads, recipesUrls, pagesUrls)
	go MkeRequest(recipesUrls, recipesHeads)
	go ParseRecipe(recipesHeads, recipesToSave)
	// save to file
	for r := range recipesToSave {
		fmt.Println(r.Name)
		fmt.Println(r.Ingredients)
		fmt.Println(r.Preparation)
		break
	}
}
