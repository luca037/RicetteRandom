package main

import (
	"encoding/xml"
	"log"
	"os"
	"strings"

	gzut "github.com/luca037/RicetteRandom/gialloZafferanoUtils"
	"golang.org/x/net/html"
)

// Massimo numero di pagine contenenti ricette.
// Fissa la dimensione di alcuni channels, non indica il numero di pagine
// da cui vengono estratte le ricette.
const kTOT_PAGES = 463

// Numero di ricette per pagina.
// Fissa la dimensione di alcuni channels.
// kTOT_PAGES * kRECIPES_PER_PAGE = # tot ricette
const kRECIPES_PER_PAGE = 15

// Path file contenente i path delle cartelle contenenti le ricette.
const kRECIPES_DIRS_PATHS = "recipesDirsPaths.txt"

// Torna il path della cartella in cui salvare i file xml delle ricette.
// Il nome viene costruito dal kURL.
func getOutPathDir(url string) string {
	s := strings.Split(url, "/")
	var path string
	if url[len(url)-1] == '/' {
		path = "ricette/" + s[len(s)-2] + "/" // se termina con /
	} else {
		path = "ricette/" + s[len(s)-1] + "/" // se termina con /
	}
	return path
}

// Ritorna una stringa in cui non sono presenti i segni di punteggiatura specificati.
func rmSomePunctuation(in string, punc ...rune) string {
	var out strings.Builder
	for _, ch := range in {
		flag := false
		for _, p := range punc {
			if ch == p {
				flag = true
			}
		}
		if flag {
			continue
		} else if ch == ' ' {
			out.WriteRune('_')
			continue
		}
		out.WriteRune(ch)
	}
	return out.String()
}

func main() {
	// gestione argomenti
	if len(os.Args) != 2 {
		log.Fatal("ERROR - Need exactly one argument (url)")
	}
	url := os.Args[1] // indirizzo di partenza (pagina 1 della portata)
	if !gzut.IsUrl(url) {
		log.Fatal("ERROR - Arg[1] isn't an url")
	}

	// salvo i link relativi alle pagine (pagine e ricette) in due channels
	pagesUrls := make(chan string, kTOT_PAGES)                         // url pag contenenti gli url delle ricette
	pagesUrls <- url                                                   // url di partenza
	pageHeads := make(chan *html.Node, len(pagesUrls))                 // head html pag contenenti url delle ricette
	recipesUrls := make(chan string, kRECIPES_PER_PAGE*len(pagesUrls)) // url delle ricette
	recipesHeads := make(chan *html.Node, len(recipesUrls))            // head html ricette
	recipesToSave := make(chan gzut.Recipe, len(recipesUrls))          // Recipe da serializzare in xml

	go gzut.MkeRequest(pagesUrls, pageHeads)
	go gzut.ParseRecipesPage(pageHeads, recipesUrls, pagesUrls)
	go gzut.MkeRequest(recipesUrls, recipesHeads)
	go gzut.ParseRecipe(recipesHeads, recipesToSave)

	// salvo dove si trova la cartella contenente i file xml (serve al programma in C++)
	outPathDir := getOutPathDir(url)
	file, err := os.OpenFile(kRECIPES_DIRS_PATHS, os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
	if err != nil {
		log.Fatal(err)
	}
	defer file.Close()
	file.Write([]byte(os.Getenv("PWD") + "/" + outPathDir + "\n")) // abs path

	// serializzazione ricette in file formato xml
	if err := os.MkdirAll(outPathDir, os.ModePerm); err != nil {
		log.Fatal(err)
	}
	for r := range recipesToSave {
		file, err := os.Create(outPathDir + rmSomePunctuation(r.Name, '"', ':', '.', ',') + ".xml")
		if err != nil {
			log.Fatal(err)
		}
		defer file.Close()
		encoder := xml.NewEncoder(file)
		encoder.Indent("", "    ")
		if err := encoder.Encode(r); err != nil {
			log.Fatal(err)
		}
	}
}
