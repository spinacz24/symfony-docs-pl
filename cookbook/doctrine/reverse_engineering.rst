.. index::
   single: Doctrine; Generating entities from existing database

Jak wygenerować encje w istniejącej bazie danych?
=================================================

Po rozpoczeciu prac nad nowym projektem który używa bazy danych,
mogą wystąþic dwie różne sytuacje. W większości przypadków,
model bazy danych jest projektowany i budowany od podstaw.
Czasami jednak zaczyna sie pracę z istniejącym już i przypuszczalnie niezmienialnym
modelem bazy danych.
Na szczęście, Doctrine posiada kilka narzędzi które pomogą wygenerować klasy
modeli z istniejącej już bazy danych.

.. note::

    Tak jak wyjaśniono to w `Doctrine tools documentation`_, `inżynieria odwrotna`_
    (*ang. reverse engineering*) jest jednorazowym procesem rozpoczynającym projekt.
    Doctrine jest wstanie przekonwertować około 70-80% ważnych
    informacji mapowania w oparciu o pola, indeksy i klucze obce. Doctrine nie
    poradzi sobie z takimi zagadnieniami jak złączenia odwrotne(*ang. inverse associations*),
    dziedziczenie (*ang. inheritance*), encje z kluczami obcymi, które są kluczami
    głównymi lub semantyczne operacje na złączeniach, takie jak kaskady lub cykle
    zdarzeń. Będzie potrzebna pewna dodatkowa praca aby dopasować specyficzne
    wymagania modelu bazy do wygenerowanych encji.

Ten poradnik zakłada że używasz prostej aplikacji bloga z następującymi tabelami:
``blog_post`` i ``blog_comment``. Komentarze są przyłaczane z wpisami poprzez
klucze obce.

.. code-block:: sql

    CREATE TABLE `blog_post` (
      `id` bigint(20) NOT NULL AUTO_INCREMENT,
      `title` varchar(100) COLLATE utf8_unicode_ci NOT NULL,
      `content` longtext COLLATE utf8_unicode_ci NOT NULL,
      `created_at` datetime NOT NULL,
      PRIMARY KEY (`id`)
    ) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

    CREATE TABLE `blog_comment` (
      `id` bigint(20) NOT NULL AUTO_INCREMENT,
      `post_id` bigint(20) NOT NULL,
      `author` varchar(20) COLLATE utf8_unicode_ci NOT NULL,
      `content` longtext COLLATE utf8_unicode_ci NOT NULL,
      `created_at` datetime NOT NULL,
      PRIMARY KEY (`id`),
      KEY `blog_comment_post_id_idx` (`post_id`),
      CONSTRAINT `blog_post_id` FOREIGN KEY (`post_id`) REFERENCES `blog_post` (`id`) ON DELETE CASCADE
    ) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

Zanim przejdziemy dalej, upewnij się czy masz odpowiednio skonfigurowaną bazę danych
w pliku ``app/config/parameters.ini`` (lub gdziekolwiek jest skonfigurowana baza danych) 
oraz że masz zainstalowany pakiet do obsługiwania klas encji. Zakładamy też, że
utworzony jest pakiet ``AcmeBlogBundle`` i znajuduje się on w ``src/Acme/BlogBundle``.

Pierwszym krokiem do zbudowania klas encji w istniejącej bazie danych jest
zainicjowanie Doctrine do introspekcji bazy danych i wygenerowanie odpowiednich
plików metadanych.
Pliki metadanych opisują klasę encji tak, aby generowana ona była na podstawie
pól tabeli.

.. code-block:: bash

    $ php app/console doctrine:mapping:import --force AcmeBlogBundle xml

Polecenie to inicjuje Doctrine do introspekcji bazy danych i generuje pliki XML
metadanych w katalogu 
``src/Acme/BlogBundle/Resources/config/doctrine/metadata/orm``. Generowane są
dwa pliki: ``BlogPost.orm.xml`` i ``BlogComment.orm.xml``.

.. tip::

    Możliwe jest także wygenerowanie plików metadanych w formacie YAML, poprzez
    zmianę ostatniego argumentu na `yml`.

Wygenerowany plik metadanych ``BlogPost.dcm.xml`` wygląda następująco:

.. code-block:: xml

    <?xml version="1.0" encoding="utf-8"?>
    <doctrine-mapping xmlns="http://doctrine-project.org/schemas/orm/doctrine-mapping" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://doctrine-project.org/schemas/orm/doctrine-mapping http://doctrine-project.org/schemas/orm/doctrine-mapping.xsd">
      <entity name="Acme\BlogBundle\Entity\BlogPost" table="blog_post">
        <id name="id" type="bigint" column="id">
          <generator strategy="IDENTITY"/>
        </id>
        <field name="title" type="string" column="title" length="100" nullable="false"/>
        <field name="content" type="text" column="content" nullable="false"/>
        <field name="createdAt" type="datetime" column="created_at" nullable="false"/>
      </entity>
    </doctrine-mapping>

Gdy są już wygenerowane pliki metadanych, można zainicjować Doctrine do zbudowania
odpowiedznich klas encji, wykonujac poniższe dwa polecenia.

.. code-block:: bash

    php app/console doctrine:mapping:import AcmeBlogBundle annotation
    php app/console doctrine:generate:entities AcmeBlogBundle

Pierwsze polecenie generuje klasy encji z adnotacją mapowania,
ale jeśli chce się korzystać z mapowania XML albo YAML, to trzeba zmienić
argument ``annotation`` na ``xml`` lub ``yml``.

.. tip::

    Jeśli chce się korzystać z adnotacji, to po uruchomieniu tych dwóch poleceń
    można bezpiecznie usunąć pliki XML lub YAML.

Na przykład, nowo utworzona klasa encji ``BlogComment`` może wyglądać następująco::
   
   // src/Acme/BlogBundle/Entity/BlogComment.php
    namespace Acme\BlogBundle\Entity;

    use Doctrine\ORM\Mapping as ORM;

    /**
     * Acme\BlogBundle\Entity\BlogComment
     *
     * @ORM\Table(name="blog_comment")
     * @ORM\Entity
     */
    class BlogComment
    {
        /**
         * @var integer $id
         *
         * @ORM\Column(name="id", type="bigint")
         * @ORM\Id
         * @ORM\GeneratedValue(strategy="IDENTITY")
         */
        private $id;

        /**
         * @var string $author
         *
         * @ORM\Column(name="author", type="string", length=100, nullable=false)
         */
        private $author;

        /**
         * @var text $content
         *
         * @ORM\Column(name="content", type="text", nullable=false)
         */
        private $content;

        /**
         * @var datetime $createdAt
         *
         * @ORM\Column(name="created_at", type="datetime", nullable=false)
         */
        private $createdAt;

        /**
         * @var BlogPost
         *
         * @ORM\ManyToOne(targetEntity="BlogPost")
         * @ORM\JoinColumn(name="post_id", referencedColumnName="id")
         */
        private $post;
    }

Jak widać, Doctrine przekonwertował wszystkie pola tabel do prywatnych pustych
właściwości klasy wraz z adnotacjami. Najbardziej imponujące jest to że wykryte
zostało złaczenie z klasą encji ``BlogPost`` w oparciu o klucz obcy.
Dlatego można odnaleść prywatną właściwość ``$post`` odwzorowana w encji ``BlogPost``
w klasie encji ``BlogComment``.

.. note::

    Jeśli chce się mieć realacje "jeden do wielu", trzeba dodać ją ręcznie do
    encji lub wygenerowac pliki XML lub YAML i dodać sekcję w okreśłonej encji
    dla definicji "jeden do wielu", definiujac fragmenty ``inversedBy`` i ``mappedBy``.

Wygenerowane encje są teraz gotowe do użycia. Owocnej pracy!

.. _`Doctrine tools documentation`: http://www.doctrine-project.org/docs/orm/2.0/en/reference/tools.html#reverse-engineering
.. _`inżynieria odwrotna`: https://pl.wikipedia.org/wiki/In%C5%BCynieria_odwrotna

