package main

import (
	"encoding/xml"
	"os"
	"strings"

	gzut "github.com/luca037/RicetteRandom/gialloZafferanoUtils"
	"golang.org/x/net/html"
)

const kURL = "https://www.giallozafferano.it/ricette-cat/Primi/pasta/"
const kTOT_PAGES = 50
const kOUT_PATH_DIR = "./ricette/"

func main() {
	// salvo i link relativi alle pagine (pagine e ricette) in due channels
	pagesUrls := make(chan string, kTOT_PAGES)
	pagesUrls <- kURL
	pageHeads := make(chan *html.Node, len(pagesUrls))
	recipesUrls := make(chan string, 15*len(pagesUrls))
	recipesHeads := make(chan *html.Node, len(recipesUrls))
	recipesToSave := make(chan gzut.Recipe, len(recipesUrls))
	// run
	go gzut.MkeRequest(pagesUrls, pageHeads)
	go gzut.ParseRecipesPage(pageHeads, recipesUrls, pagesUrls)
	go gzut.MkeRequest(recipesUrls, recipesHeads)
	go gzut.ParseRecipe(recipesHeads, recipesToSave)
	// save to file
	if err := os.MkdirAll(kOUT_PATH_DIR, os.ModePerm); err != nil {
		panic(err)
	}
	for r := range recipesToSave {
		file, err := os.Create(kOUT_PATH_DIR + strings.ReplaceAll(r.Name, " ", "_") + ".xml")
		if err != nil {
			panic(err)
		}
		defer file.Close()
		encoder := xml.NewEncoder(file)
		encoder.Indent("", "    ")
		if err := encoder.Encode(r); err != nil {
			panic(err)
		}
	}
}
