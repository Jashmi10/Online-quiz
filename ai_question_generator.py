import openai

# Set your OpenAI API key
openai.api_key = 'your-api-key'

def generate_question():
    prompt = "Generate a multiple-choice quiz question with 4 options and the correct answer."
    
    response = openai.ChatCompletion.create(
        model="gpt-4",  # Or "gpt-3.5-turbo" if you want a smaller model
        messages=[
            {"role": "system", "content": "You are a helpful quiz generator."},
            {"role": "user", "content": prompt}
        ]
    )
    
    message = response['choices'][0]['message']['content']
    question_parts = message.split("\n")

    question_text = question_parts[0]  # The question text
    options = question_parts[1:5]     # The 4 options
    correct_option = int(question_parts[5])  # The correct option index

    return question_text, options, correct_option

if __name__ == "__main__":
    question, options, correct = generate_question()
    print(question)
    for i, option in enumerate(options):
        print(f"{i + 1}. {option}")
    print(f"Correct answer: {correct}")
