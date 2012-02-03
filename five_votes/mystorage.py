from google.appengine.ext import db

#
# Letters have a language, and can have translations
#
# Signers sign a letter and language.
#
# When deleting a translation, all the signers to its
# language also go.
#
#

#
# User (letter owner)
#
class User(db.Model):
    fb_id = db.IntegerProperty()
    created = db.DateTimeProperty(auto_now=True)
    name = db.StringProperty()


#
# Match
#
class Match(db.Model):
    owner = db.ReferenceProperty(User)
    title = db.StringProperty()
    url_name = db.StringProperty()
    desc = db.StringProperty(multiline=True)
    extended_desc = db.TextProperty()
    num_ideas = db.IntegerProperty()

#
#
class Idea(db.Model):
    owner = db.ReferenceProperty(User)
    match = db.ReferenceProperty(Match)
# max 80 chars
    title = db.StringProperty()
# max 300 chars
    desc = db.StringProperty()
# several pages long (10000 chars)
    extended_desc = db.TextProperty()
    num = db.IntegerProperty()

#
#
#
class Vote(db.Model):
    idea = db.ReferenceProperty(Idea)
    match = db.ReferenceProperty(Match)
    owner = db.ReferenceProperty(User)
    num_votes = db.IntegerProperty()
    rating = db.StringProperty()


#
#
#
#class Signer(db.Model):
#    letter = db.ReferenceProperty(Letter)
#    lang = db.StringProperty()
#    fb_id = db.IntegerProperty()
#    comment = db.TextProperty()
#    created = db.DateTimeProperty(auto_now=True)
#    name = db.StringProperty()
#    pic_url = db.StringProperty()
#    picture = db.BlobProperty()
#    location = db.StringProperty()
#    cc1 = db.StringProperty()
#
#
