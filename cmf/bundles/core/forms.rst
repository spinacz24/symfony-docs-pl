.. index::
    single: rdzeń; pakiety; typy formularzy

Typy formularzy dostarczane przez CoreBundle
============================================

Etykieta pola wyboru URL
------------------------

Typ formularza ``cmf_core_checkbox_url_label`` oparty jest na typie ``checkbox``
i dodaje przydatne funkcje do klasycznej kontrolki "akceptuję regulamin". Różnicą
ze zwykłym polem wybory jest to, ze etykieta jest linkowana do dokumentu, zwykle
do dokumentu "Regulamin". W przypadku z korzystania z tego typu, można dodatkowo
określić ``content_ids``, co jest rozumiane przez
:doc:`DynamicRouter <../routing/dynamic>`, wraz z wymiennymi tokenami::

    $form->add('terms', 'cmf_core_checkbox_url_label', array(
        'label' => 'I have seen the <a href="%team%">Team</a> and <a href="%more%">More</a> pages ...',
        'content_ids' => array(
            '%team%' => '/cms/content/static/team',
            '%more%' => '/cms/content/static/more',
        ),
    ));

Ten typ formularza automatycznie generuje trasy dla określonej treści i przekazuje
trasy o helpera Twig ``trans`` dla zastąpienia w etykiecie.
