// -------------------------------------------------------------------------------------------------
// OpenSeaMap Water Depth - Web frontend for depth data handling.
//
// Written in 2012 by Dominik Fässler dfa@bezono.org
//
// To the extent possible under law, the author(s) have dedicated all copyright
// and related and neighboring rights to this software to the public domain
// worldwide. This software is distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along
// with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
// -------------------------------------------------------------------------------------------------

OSeaM.models.Frontend = Backbone.Model.extend({
    actualView:null,
    translations: {},
    getAuth: function() {
        if (this.has('auth') === false) {
            this.set({
                auth: new OSeaM.models.Auth()
            });
        }
        return this.get('auth');
    },
    getTracks: function() {
        if (this.has('tracks') === false) {
            this.set({
                tracks: new OSeaM.models.Tracks()
            });
        }
        return this.get('tracks');
    },
    getVessels: function() {
        if (this.has('vessels') === false) {
            this.set({
                vessels: new OSeaM.models.Vessels()
            });
        }
        return this.get('vessels');
    },
    startView: function(name, settings) {
        if (this.actualView) {
            this.actualView.close();
        }
        var cfg = settings || {};
        this.actualView = new OSeaM.views[name](jQuery.extend({
            el: OSeaM.container
        }, cfg));
        this.actualView.render();
    },
    setLanguage: function(language) {
        this.set({language: language});
        if (this.translations[language] === undefined) {
            var me = this;
            jQuery.ajax({
                url: 'translations/' + language + '.json',
                dataType: 'json',
                success: function(data, success, jqXHR) {
                    me.translations[language] = data;
                    me.translate($('body'));
                },
                error: function(jqXHR, textStatus, errorThrown) {
                    alert('Unable to load language.');
                }
            });
        } else {
            this.set({language: language});
            this.translate($('body'));
        }
    },
    getLanguage: function() {
        return this.get('language');
    },
    translate: function(el) {
        // Seach in children
        elements = el.find('[data-trt], [data-trt-placeholder]');
        for (var i = 0; i < elements.length; i++) {
            this.translateEl(elements[i]);
        }
        // Seach on top level
        elements = el.filter('[data-trt], [data-trt-placeholder]');
        for (var i = 0; i < elements.length; i++) {
            this.translateEl(elements[i]);
        }
    },
    translateEl: function(el) {
        var jEl = $(el);
        var dataTrt = jEl.data('trt') || '0000:Unknown';
        dataTrt = dataTrt.split(':', 2);
        jEl.html(this.getPhrase(dataTrt[0]));
        var dataTrt = jEl.data('trt-placeholder') || null;
        if (dataTrt) {
            dataTrt = dataTrt.split(':', 2);
            jEl.attr('placeholder', this.getPhrase(dataTrt[0]));
        }
    },
    translateAttr: function(jEl, attr, text) {
        dataTrt = text.split(':', 2);
        jEl.attr(attr, this.getPhrase(dataTrt[0]));
    },
    getPhrase: function(id) {
        var language = this.getLanguage();
        if (this.translations[language] === undefined) {
            return '';
        }
        if (this.translations[language][id] === undefined) {
            return '::unknown::';
        }
        return this.translations[language][id];
    }
});
