.. highlight:: php
   :linenothreshold: 2

.. index::
   single: polecenia konsoli; generate:doctrine:crud
   single: generowanie kontrolera CRUD
   
Generowanie kontrolera CRUD opartego na encji Doctrine
------------------------------------------------------

Stosowanie
~~~~~~~~~~

Polecenie **generate:doctrine:crud** generuje podstawowy kontroler dla danej encji
zlokalizowanej w określonym pakiecie. Kontroler ten pozwala na wykonanie pięciu podstawowych
operacji na modelu.

*  Listowanie wszystkich rekordów;
*  Pokazywanie jednego rekordu identyfikowanego przez jego klucz podstawowy;
*  Tworzenie nowego rekordu;
*  Edytowanie istniejącego rekordu;
*  Usuwanie istniejącego rekodu.

Domyślnie polecenie to jest uruchamiane w trybie interaktywnym i zadaje pytania
w celu określenia nazwy encji, przedrostka trasy lub czy należy wygenerować akcje
zapisu:

.. code-block:: bash
   
   $ php app/console generate:doctrine:crud
   
Aby wyłączyć tryb interaktywny, trzeba zastosować opcję ``--no-interaction``, nie
zapominając przekazać inne potrzebne opcje:

.. code-block:: bash
   
   $ php app/console generate:doctrine:crud --entity=AcmeBlogBundle:Post --format=annotation --with-write --no-interaction

   
Dostępne opcje
~~~~~~~~~~~~~~

*  ``--entity``: Nazwa encji, podana w notacji skrótowej, zawierająca nazwę pakietu
   w którym umieszczona jest encja i nazwę encji. Na przykład ``AcmeBlogBundle:Post``:
   
   .. code-block:: bash
      
      $ php app/console generate:doctrine:crud --entity=AcmeBlogBundle:Post

*  ``--route-prefix``: Przedrostek używany dla każdej trasy, która identyfikuje akcję:
      
   .. code-block:: bash
      
      $ php app/console generate:doctrine:crud --route-prefix=acme_post
      

*  ``--with-write``: (**no**) [wartości: yes|no] Czy generować akcje 'new',
   'create', 'edit', 'update' i 'delete':
   
   .. code-block:: bash
      
      $ php app/console generate:doctrine:crud --with-write
      
*  ``--format``: (**adnotacja**) [wartości: yml, xml, php lub annotation] Określenie
   formatu, jaki zostanie użyty do generowania plików konfiguracyjnych, takich jak
   trasowanie. Domyślnie w poleceniu stosowane są adnotacje. Wybór formatu adnotacji
   wymaga wcześniejszego zainstalowania pakietu SensioFrameworkExtraBundle:
   
   .. code-block:: bash
   
      $ php app/console generate:doctrine:crud --format=annotation
      
