from google.appengine.ext import db

class User(db.Model):
    fb_id = db.IntegerProperty()
    created = db.DateTimeProperty(auto_now=True)
    email_notifs = db.BooleanProperty
    fb_added = db.BooleanProperty
    name = db.StringProperty
    email = db.EmailProperty


class Question(db.Model):
    owner = db.ReferenceProperty(User)
    question_text = db.StringProperty(multiline=True)
    extended_desc = db.TextProperty
    ideas_last_date = db.DateTimeProperty
    closing_date = db.IntegerProperty           # days after closing date
    voting_from_beginning  = db.BooleanProperty # 1 = from_beginning, 0 = after_ideas_last_date)
    show_votes_as_they_happen = db.BooleanProperty # false = show at end

    show_who_votes_for_what = db.BooleanProperty
    use_aliases = db.BooleanProperty
    reveal_alias_identities = db.BooleanProperty   # true = reveal at end
    question_prize = db.StringProperty
    users_can_invite_others = db.BooleanProperty
    public_participation    = db.BooleanProperty

class QuestionUsers(db.Model):
    question = db.ReferenceProperty(Question)
    user = db.ReferenceProperty(User)
    votes_left = db.IntegerProperty

class Invitations(db.Model):
    question = db.ReferenceProperty(Question)
    email = db.EmailProperty
    invitation_id = db.IntegerProperty()
    

