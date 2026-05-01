// Mock message data for watch prototype
// 5 conversations, 10 contacts, 40 messages total

#ifndef MOCK_MESSAGES_H
#define MOCK_MESSAGES_H

// === Contacts ===
struct Contact {
  int id;
  const char* name;
  const char* phone;
};

static const Contact contacts[] = {
  {0, "Me", "+1555000000"},
  {1, "Mom", "+1555000001"},
  {2, "Dad", "+1555000002"},
  {3, "Jamie", "+1555000003"},
  {4, "Alex", "+1555000004"},
  {5, "Sam", "+1555000005"},
  {6, "Rosa", "+1555000006"},
  {7, "UPS", "+1555000007"},
  {8, "Amazon", "+1555000008"},
  {9, "FedEx", "+1555000009"},
};

static const int CONTACT_COUNT = 10;
static const int ME = 0;

// === Messages ===
struct Message {
  int senderId;  // index into contacts[]
  const char* text;
};

struct Conversation {
  const char* name;
  const int* participantIds;
  int participantCount;
  const Message* messages;
  int messageCount;
};

// === Conversation 1: Family Group ===
static const int familyParticipants[] = {0, 1, 2};  // Me, Mom, Dad
static const Message familyMessages[] = {
  {1, "Are you coming to dinner Sunday?"},
  {0, "Yes! What time?"},
  {2, "6pm, bringing the grill out"},
  {1, "Can you pick up ice on the way?"},
  {0, "Sure, bags or cubes?"},
  {1, "Bags please, 2 of them"},
  {2, "Also grab charcoal if you see it"},
  {0, "Got it, see you then"},
};

// === Conversation 2: Jamie ===
static const int jamieParticipants[] = {0, 3};  // Me, Jamie
static const Message jamieMessages[] = {
  {3, "Hey did you see the game last night?"},
  {0, "No I missed it! Who won?"},
  {3, "Warriors pulled it off in OT"},
  {3, "Curry went off in the 4th"},
  {0, "Classic. Need to watch the highlights"},
};

// === Conversation 3: Work - Alex & Sam ===
static const int workParticipants[] = {0, 4, 5};  // Me, Alex, Sam
static const Message workMessages[] = {
  {4, "Quick sync before the 2pm?"},
  {0, "Sure, 10 min?"},
  {5, "Can I join? Have questions about the API"},
  {4, "Yeah let's do 1:45 in the small room"},
  {0, "Works for me"},
  {5, "Perfect, bringing my laptop"},
  {0, "See you there"},
};

// === Conversation 4: Rosa ===
static const int rosaParticipants[] = {0, 6};  // Me, Rosa
static const Message rosaMessages[] = {
  {6, "The kids loved the present btw"},
  {6, "Leo has been playing with it nonstop"},
  {0, "Ha! Glad it was a hit"},
  {6, "We should get together soon"},
  {0, "Definitely, maybe next weekend?"},
  {6, "Saturday works, park day?"},
  {0, "Perfect, morning or afternoon?"},
  {6, "Morning, before it gets too hot"},
};

// === Conversation 5: Delivery Updates ===
static const int deliveryParticipants[] = {0, 7, 8, 9};  // Me, UPS, Amazon, FedEx
static const Message deliveryMessages[] = {
  {7, "Your package is out for delivery"},
  {0, "Finally!"},
  {7, "Delivered to front door at 2:34pm"},
  {8, "Your order has shipped"},
  {0, "Which one?"},
  {8, "Order #827: Silicone sheet 12x12"},
  {0, "Oh nice, the watch stuff"},
  {9, "Delivery scheduled for tomorrow"},
  {0, "Thanks"},
  {9, "Package delivered"},
  {0, "Got it"},
  {8, "How was your delivery?"},
};

// === All Conversations ===
static const Conversation conversations[] = {
  {"Family Group", familyParticipants, 3, familyMessages, 8},
  {"Jamie", jamieParticipants, 2, jamieMessages, 5},
  {"Work Chat", workParticipants, 3, workMessages, 7},
  {"Rosa", rosaParticipants, 2, rosaMessages, 8},
  {"Deliveries", deliveryParticipants, 4, deliveryMessages, 12},
};

static const int CONVERSATION_COUNT = 5;

// Helper to check if a message is from "Me"
inline bool isFromMe(const Message& msg) {
  return msg.senderId == ME;
}

// Helper to get sender name
inline const char* getSenderName(const Message& msg) {
  return contacts[msg.senderId].name;
}

#endif // MOCK_MESSAGES_H
